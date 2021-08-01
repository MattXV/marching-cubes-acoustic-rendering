#include "ImageSource.h"

namespace unda {
	namespace acoustics {

		template<typename T>
		double MicrophoneAttenuation(T x, T y, T z, std::array<T, 2> angle, char mtype) {
			if (mtype == 'b' || mtype == 'c' || mtype == 's' || mtype == 'h') {
				double gain, vartheta, varphi, rho;

				// Polar Pattern         rho
				// ---------------------------
				// Bidirectional         0
				// Hypercardioid         0.25    
				// Cardioid              0.5
				// Subcardioid           0.75
				// Omnidirectional       1

				switch (mtype) {
				case 'b':
					rho = 0;
					break;
				case 'h':
					rho = 0.25;
					break;
				case 'c':
					rho = 0.5;
					break;
				case 's':
					rho = 0.75;
					break;
				};

				vartheta = acos(z / sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)));
				varphi = atan2(y, x);

				gain = sin(M_PI / 2 - angle[1]) * sin(vartheta) * cos(angle[0] - varphi) + cos(M_PI / 2 - angle[1]) * cos(vartheta);
				gain = rho + (1 - rho) * gain;

				return T(gain);
			}
			else {
				return T(1);
			}
		}



		ImageSourceModel::ImageSourceModel(const std::array<double, 3>& _spaceDimensions, const std::array<double, 3>& _sourcePosition, const std::array<double, 3>& _receiverPosition,	std::array<std::array<double, 6>, 6>& _surfaceReflection, int _nSamples, unsigned int _order)
			: spaceDimensions(_spaceDimensions)
			, sourcePosition(_sourcePosition)
			, receiverPosition(_receiverPosition)
			, surfaceReflection(_surfaceReflection)
			, order(_order)
		{
			updateParameters();
		}

		void ImageSourceModel::dispatchCPUThreads()
		{

			int points_x = (int)ceil(nSamples / (2.0 * room[0]));
			int points_y = (int)ceil(nSamples / (2.0 * room[1]));
			int points_z = (int)ceil(nSamples / (2.0 * room[2]));

			unsigned int nThreads = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 1;
			std::queue<std::function<void()>> tasks;
			std::mutex mutex;

			for (int x = -points_x; x <= points_x; x++) {
				for (int y = -points_y; y <= points_y; y++) {
					for (int z = -points_z; z <= points_z; z++) {
						std::function<void()> task = [&, x, y, z]()->void { computeReflections(x, y, z); };
						tasks.push(task);
					}
				}
			}
			std::function<void()> consumer = [&]() { 
				std::function<void()> compute;
				while (1) {
					if (mutex.try_lock()) {
						if (tasks.empty()) { mutex.unlock(); break; }
						compute = std::move(tasks.front());
						tasks.pop();
						mutex.unlock();
						compute();
					}
				}
			};
			for (unsigned int thread = 0; thread < nThreads; thread++) {
				workers.push_back(std::thread(consumer));
			}

			for (std::thread& th : workers) th.join();

			computeTail();
		}

		void ImageSourceModel::updateParameters()
		{
			double volume = spaceDimensions[0] * spaceDimensions[1] * spaceDimensions[2];
			double floorSurface = spaceDimensions[0] * spaceDimensions[2];
			double ceilingSurface = spaceDimensions[0] * spaceDimensions[2];
			double backWallSurface = spaceDimensions[1] * spaceDimensions[0];
			double frontWallSurface = spaceDimensions[1] * spaceDimensions[0];
			double leftWallSurface = spaceDimensions[1] * spaceDimensions[2];
			double rightWallSurface = spaceDimensions[1] * spaceDimensions[2];

			double totalAlpha = 0.0;
			std::array<double, 6> frequencyDependentT60;
			for (int bin = 0; bin < 6; bin++) {
				// Using Sabine's equation to determine space reverberation if n_samples is not known.
				double alpha =
					floorSurface * (1 - pow(surfaceReflection[0][bin], 2)) + // floor
					ceilingSurface * (1 - pow(surfaceReflection[1][bin], 2)) + // celing
					backWallSurface * (1 - pow(surfaceReflection[2][bin], 2)) + // back wall
					frontWallSurface * (1 - pow(surfaceReflection[3][bin], 2)) + // front wall
					leftWallSurface * (1 - pow(surfaceReflection[4][bin], 2)) + // left wall
					rightWallSurface * (1 - pow(surfaceReflection[5][bin], 2));  // right wall
				totalAlpha += alpha;
				frequencyDependentT60[bin] = 0.161 * (volume / alpha);
			}
			totalAlpha /= 6.0;
			t_60 = 0.161 * (volume / totalAlpha);
			totalSurface = floorSurface + ceilingSurface + backWallSurface + frontWallSurface + leftWallSurface + rightWallSurface;
			meanFreePathEstimate = meanFreePath(volume, (double)totalSurface);


			UNDA_LOG_MESSAGE("T60: " + std::to_string(frequencyDependentT60[0])
				+ ", " + std::to_string(frequencyDependentT60[1]) + ", "
				+ std::to_string(frequencyDependentT60[2]) + ", "
				+ std::to_string(frequencyDependentT60[3]) + ", "
				+ std::to_string(frequencyDependentT60[4]) + ", "
				+ std::to_string(frequencyDependentT60[5]) + ", ");
			UNDA_LOG_MESSAGE("Predicted total t60: " + std::to_string(t_60));
			UNDA_LOG_MESSAGE("Volume: " + std::to_string(volume));
	
			if (nSamples == 0) nSamples = (int)((std::ceil(t_60)) * samplingFrequency);
			for (int bin = 0; bin < 6; bin++) {
				irs[bin].resize(nSamples);
			}

			source[0]   = sourcePosition[0] / timeStep;
			source[1]   = sourcePosition[1] / timeStep;
			source[2]   = sourcePosition[2] / timeStep;
			listener[0] = receiverPosition[0] / timeStep;
			listener[1] = receiverPosition[1] / timeStep;
			listener[2] = receiverPosition[2] / timeStep;
			room[0]     = spaceDimensions[0] / timeStep;
			room[1]     = spaceDimensions[1] / timeStep;
			room[2]     = spaceDimensions[2] / timeStep;
		}

		void ImageSourceModel::computeReflections(int x, int y, int z) {
			double Rm[3];
			double Rp_plus_Rm[3];
			double reflections[3][8];  // multidimensional array N x 3; N -> octave bands
			double distance;

			Rm[0] = 2 * (double)x * room[0];
			Rm[1] = 2 * (double)y * room[1];
			Rm[2] = 2 * (double)z * room[2];

			for (int q = 0; q <= (int)order; q++)
			{
						Rp_plus_Rm[0] = (1 - 2 * (double)q) * source[0] - listener[0] + Rm[0];
						for (int bin = 0; bin < 6; bin++) {
							reflections[0][bin] = pow(surfaceReflection[0][bin], std::abs(x - q)) * pow(surfaceReflection[1][bin], std::abs(x));
						}

				for (int j = 0; j <= (int)order; j++)
				{
						Rp_plus_Rm[1] = (1 - 2 * (double)j) * source[1] - listener[1] + Rm[1];
						for (int bin = 0; bin < 6; bin++) {
							reflections[1][bin] = pow(surfaceReflection[2][bin], std::abs(y - j)) * pow(surfaceReflection[3][bin], std::abs(y));
						} 

					for (int k = 0; k <= (int)order; k++)
					{
						Rp_plus_Rm[2] = (1 - 2 * (double)k) * source[2] - listener[2] + Rm[2];
						for (int bin = 0; bin < 6; bin++) {
							reflections[2][bin] = pow(surfaceReflection[4][bin], std::abs(z - k)) * pow(surfaceReflection[5][bin], std::abs(z));
						}

						distance = sqrt(pow(Rp_plus_Rm[0], 2) + pow(Rp_plus_Rm[1], 2) + pow(Rp_plus_Rm[2], 2));
						int startPosition = (int)floor(distance);
						if (startPosition >= 0 && startPosition < nSamples) {
							for (int bin = 0; bin < 6; bin++) {
								Sample attenuation = (Sample)MicrophoneAttenuation(Rp_plus_Rm[0], Rp_plus_Rm[1], Rp_plus_Rm[2], microphoneAngle, 'o');
								irs[bin][startPosition] += (attenuation * (Sample)reflections[0][bin] * (Sample)reflections[1][bin] * (Sample)reflections[2][bin]) / (Sample(4) * (Sample)M_PI * (Sample)distance * (Sample)timeStep);
							}
						}
					}
				}
			}
		}


		void ImageSourceModel::computeTail()
		{
			for (int i = 0; i < 6; i++)
				WriteAudioFile({ irs[i] }, "frequency_bin_" + std::to_string(i) + ".wav");
			{
				Filter filter = Filter(Filter::filterType::BPF, 20, 125);
				filter.convolveToSignal(irs[0]);
			}
			{
				Filter filter = Filter(Filter::filterType::BPF, 125, 250);
				filter.convolveToSignal(irs[1]);
			}
			{
				Filter filter = Filter(Filter::filterType::BPF, 250, 500);
				filter.convolveToSignal(irs[2]);
			}
			{
				Filter filter = Filter(Filter::filterType::BPF, 500, 1000);
				filter.convolveToSignal(irs[3]);
			}
			{
				Filter filter = Filter(Filter::filterType::BPF, 1000, 2000);
				filter.convolveToSignal(irs[4]);
			}
			{
				Filter filter = Filter(Filter::filterType::BPF, 2000, 20000);
				filter.convolveToSignal(irs[5]);
			}

			output.clear();
			output.resize(irs[0].size());
			for (int value = 0; value < output.size(); value++)
				output[value] = irs[0][value] + irs[1][value] + irs[2][value] + irs[3][value] + irs[4][value] + irs[5][value];

			NormaliseSignal(output);
			WriteAudioFile({ output }, "ir.wav", samplingFrequency);
		}
	}
}