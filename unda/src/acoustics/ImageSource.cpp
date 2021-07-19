#include "ImageSource.h"

namespace unda {
	namespace acoustics {


		double sim_microphone(double x, double y, double z, double* angle, char mtype) {
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

				return gain;
			}
			else {
				return 1;
			}
		}



		ImageSourceModel::ImageSourceModel(int _nThreads, const std::array<double, 3>& _spaceDimensions, const std::array<double, 3>& _sourcePosition, const std::array<double, 3>& _receiverPosition,
			std::array<std::array<double, 6>, 6>& _surfaceReflection, int _nSamples, unsigned int _order)
			: spaceDimensions(_spaceDimensions)
			, sourcePosition(_sourcePosition)
			, receiverPosition(_receiverPosition)
			, surfaceReflection(_surfaceReflection)
			, order(_order)
		{
			if (nThreads > 32) { throw std::invalid_argument("Invalind number of threads!"); return; }
			nThreads = _nThreads;
			samplingFrequency = unda::sampleRate;
			speedOfSound = unda::maths::c;

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
					floorSurface     * (1 - pow(surfaceReflection[0][bin], 2)) + // floor
					ceilingSurface   * (1 - pow(surfaceReflection[1][bin], 2)) + // celing
					backWallSurface  * (1 - pow(surfaceReflection[2][bin], 2)) + // back wall
					frontWallSurface * (1 - pow(surfaceReflection[3][bin], 2)) + // front wall
					leftWallSurface  * (1 - pow(surfaceReflection[4][bin], 2)) + // left wall
					rightWallSurface * (1 - pow(surfaceReflection[5][bin], 2));  // right wall
				totalAlpha += alpha;
				frequencyDependentT60[bin] = 0.161 * (volume / alpha);
			}
			totalAlpha /= 6.0;
			t_60 = 0.161 * (volume / totalAlpha);
			totalSurface = floorSurface + ceilingSurface + backWallSurface + frontWallSurface + leftWallSurface + rightWallSurface;
			meanFreePathEstimate = meanFreePath(volume, (double)totalSurface);

			//UNDA_LOG_MESSAGE("T60: " + std::to_string(frequencyDependentT60[0])
			//	+ ", " + std::to_string(frequencyDependentT60[1]) + ", "
			//	+ std::to_string(frequencyDependentT60[2]) + ", "
			//	+ std::to_string(frequencyDependentT60[3]) + ", "
			//	+ std::to_string(frequencyDependentT60[4]) + ", "
			//	+ std::to_string(frequencyDependentT60[5]) + ", ");
			//UNDA_LOG_MESSAGE("Predicted total t60: " + std::to_string(t_60));
			//UNDA_LOG_MESSAGE("Volume: " + std::to_string(volume));

		}

		ImageSourceModel::~ImageSourceModel()
		{

		}

		void ImageSourceModel::generateIR()
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

			utils::Timer timer("Office - Generating RIR");
			timer.setInfo(" RIR . Estimated t60:" + std::to_string(t_60));

			timer.start();
			//computeIRs(16);
			for (int bin = 0; bin < 6; bin++) {
				irs[bin].resize(nSamples);
			}
			for (int thread = 0; thread < nThreads; thread++) {
				std::function lambda = [this](int threadId) { computeIRs(threadId);	};
				workers.push_back(std::thread(lambda, thread));
			}
			for (std::thread& thread : workers)
				thread.join();
			timer.stop();

			computeTail();
		}

		void ImageSourceModel::doPatches(const std::vector<SurfacePatch>& patches, int cellsPerDimesion)
		{
			long double patchContribution = totalSurface / std::pow((long double)cellsPerDimesion, 3);
			for (SurfacePatch patch : patches) {
				if (patch.confidence < 0.90f) {
					continue;
				}
				//
				//if (patch.name.find("bottomFace") != std::string::npos) {

				for (int wall = 0; wall < 6; wall++) {
					for (int i = 0; i < 6; i++) {
						long double patchCoefficient = (long double)patch.coefficients[i];
						patchCoefficient = (long double)alphaToBeta(patchCoefficient);
						//surfaceReflection[wall][i] = std::lerp((long double)surfaceReflection[0][i], patchCoefficient, patchContribution);
						surfaceReflection[wall][i] = (surfaceReflection[wall][i] + patchCoefficient) / 2;
					}
				}
				

				//if (patch.name.find("topFace") != std::string::npos) {

				//	for (int i = 0; i < 6; i++) {
				//		long double patchCoefficient = (long double)patch.coefficients[i];
				//		patchCoefficient = (long double)alphaToBeta(patchCoefficient);
				//		surfaceReflection[1][i] = std::lerp((long double)surfaceReflection[1][i], patchCoefficient, patchContribution);
				//	}
				//}

				//if (patch.name.find("backFace") != std::string::npos) {

				//	for (int i = 0; i < 6; i++) {
				//		long double patchCoefficient = (long double)patch.coefficients[i];
				//		patchCoefficient = (long double)alphaToBeta(patchCoefficient);
				//		surfaceReflection[2][i] = std::lerp((long double)surfaceReflection[2][i], patchCoefficient, patchContribution);
				//	}
				//}
				//if (patch.name.find("frontFace") != std::string::npos) {

				//	for (int i = 0; i < 6; i++) {
				//		long double patchCoefficient = (long double)patch.coefficients[i];
				//		patchCoefficient = (long double)alphaToBeta(patchCoefficient);
				//		surfaceReflection[3][i] = std::lerp((long double)surfaceReflection[3][i], patchCoefficient, patchContribution);
				//	}
				//}
				//if (patch.name.find("leftFace") != std::string::npos) {

				//	for (int i = 0; i < 6; i++) {
				//		long double patchCoefficient = (long double)patch.coefficients[i];
				//		patchCoefficient = (long double)alphaToBeta(patchCoefficient);
				//		surfaceReflection[4][i] = std::lerp((long double)surfaceReflection[4][i], patchCoefficient, patchContribution);
				//	}
				//}

				//if (patch.name.find("rightFace") != std::string::npos) {

				//	for (int i = 0; i < 6; i++) {
				//		long double patchCoefficient = (long double)patch.coefficients[i];
				//		patchCoefficient = (long double)alphaToBeta(patchCoefficient);
				//		surfaceReflection[5][i] = std::lerp((long double)surfaceReflection[5][i], patchCoefficient, patchContribution);
				//	}
				//}
			}
		}



		void ImageSourceModel::computeIRs(int threadId)
		{
			// Temporary variables and constants (high-pass filter)
			//const double W = 2 * M_PI * 100 / samplingFrequency; // The cut-off frequency equals 100 Hz
			//const double R1 = exp(-W);
			//const double B1 = 2 * R1 * cos(W);
			//const double B2 = -R1 * R1;
			//const double A1 = -(1 + R1);
			//double       X0;
			//double*		 Y = new double[3];

			// Temporary variables and constants (image-method)

			double		 source[3];
			double		 listener[3];
			double	     room[3];
			double       Rm[3];
			double       Rp_plus_Rm[3];

			double		 reflections[3][8];  // multidimensional array N x 3; N -> octave bands
			double       fdist, dist;
			double       gain;

			variablesMutex.lock();
			const double Fc = 1.0; // The cut-off frequency equals fs/2 - Fc is the normalized cut-off frequency.
			const int    Tw = 2 * ROUND(0.004 * samplingFrequency); // The width of the low-pass FIR equals 8 ms
			double*		 LPI = new double[Tw];
			const double timeStep = speedOfSound / samplingFrequency;

			source[0] = sourcePosition[0] / timeStep;
			source[1] = sourcePosition[1] / timeStep;
			source[2] = sourcePosition[2] / timeStep;
			listener[0] = receiverPosition[0] / timeStep;
			listener[1] = receiverPosition[1] / timeStep;
			listener[2] = receiverPosition[2] / timeStep;
			room[0] = spaceDimensions[0] / timeStep;
			room[1] = spaceDimensions[1] / timeStep;
			room[2] = spaceDimensions[2] / timeStep;

			// Image Source Model dimensions:
			// 
			// number of points computed along the Y axis
			//int points_x = (int)ceil(nSamples / (2.0 * L[0]));
			//int points_y = (int)ceil(nSamples / (2.0 * L[1]));
			//int points_z = (int)ceil(nSamples / (2.0 * L[2]));
			int points_x = (int)ceil(nSamples / (2.0 * room[0]));
			int points_y = (int)ceil(nSamples / (2.0 * room[1]));
			int points_z = (int)ceil(nSamples / (2.0 * room[2]));

			
			//
			// Each thread is assigned with a slice of the cube2 *  representing the space
			// The cubes has as many slices as number of threads.
			// Each thread still computes the other two dimensions, it just operates at
			// a different height.
			// 
			int startIndex, endIndex;
			int pointsPerThread = (int)floor((((double)points_y * 2) + 1) / (double)nThreads);
			startIndex = (threadId * pointsPerThread) - points_y;
			endIndex = startIndex + pointsPerThread;
			if (threadId == (nThreads - 1))	endIndex = points_y;

			double angle[2] = { microphoneAngle[0], microphoneAngle[1] };
			variablesMutex.unlock();

			// Generate room impulse response
			for (int mx = -points_x; mx <= points_x; mx++)
			{
				        Rm[0] = 2 * (double)mx * room[0];

				for (int my = startIndex; my < endIndex; my++) 
				{
					    Rm[1] = 2 * (double)my * room[1];

					for (int mz = -points_z; mz <= points_z; mz++)
					{
						Rm[2] = 2 * (double)mz * room[2];

						for (int q = 0; q <= (int)order; q++)
						{
							Rp_plus_Rm[0] = (1 - 2 * (double)q) * source[0] - listener[0] + Rm[0];

							// Frequency-dependent relfection calculation :D
							for (int bin = 0; bin < 6; bin++) {
								reflections[0][bin] = pow(surfaceReflection[0][bin], abs(mx - q)) * pow(surfaceReflection[1][bin], abs(mx));
							}

							for (int j = 0; j <= (int)order; j++)
							{
								Rp_plus_Rm[1] = (1 - 2 * (double)j) * source[1] - listener[1] + Rm[1];
								for (int bin = 0; bin < 6; bin++) {
									reflections[1][bin] = pow(surfaceReflection[2][bin], std::abs(my - j)) * pow(surfaceReflection[3][bin], std::abs(my));
								}

								for (int k = 0; k <= (int)order; k++)
								{
									Rp_plus_Rm[2] = (1 - 2 * (double)k) * source[2] - listener[2] + Rm[2];

									for (int bin = 0; bin < 6; bin++) {
										reflections[2][bin] = pow(surfaceReflection[4][bin], std::abs(mz - k)) * pow(surfaceReflection[5][bin], std::abs(mz));
									}

									dist = sqrt(pow(Rp_plus_Rm[0], 2) + pow(Rp_plus_Rm[1], 2) + pow(Rp_plus_Rm[2], 2));
									fdist = floor(dist);

									if (fdist < nSamples)
									{
										std::array<double, 6> gains;

										double microphone = sim_microphone(Rp_plus_Rm[0], Rp_plus_Rm[1], Rp_plus_Rm[2], angle, 'o');
										for (int bin = 0; bin < 6; bin++) {
											gain = (microphone * reflections[0][bin] * reflections[1][bin] * reflections[2][bin]) / (4 * M_PI * dist * timeStep);
											gains[bin] = (double)gain;
										}

										for (int n = 0; n < Tw; n++)
											LPI[n] = 0.5 * (1 - cos(2 * M_PI * (((double)n + 1 - (dist - fdist)) / Tw))) * Fc * sinc(M_PI * Fc * ((double)n + 1 - (dist - fdist) - (Tw / 2)));

										int startPosition = (int)fdist - (Tw / 2) + 1;
										for (int n = 0; n < Tw; n++)
											for (int bin = 0; bin < 6; bin++)
												if (startPosition + n >= 0 && startPosition + n < nSamples)
													irs[bin][startPosition + (size_t)n] += (float)gains[bin] * (float)LPI[n];
									}	
								}
							}
						}
					}
				}
			} // End of RIR computation
			delete[] LPI;
		}

		void ImageSourceModel::computeTail()
		{
			//for (int i = 0; i < 6; i++)
				//WriteAudioFile({ irs[i] }, "frequency_bin_" + std::to_string(i) + ".wav", samplingFrequency);
			{
				Filter filter = Filter(Filter::filterType::BPF, 20, 125);
				irs[0] = FFTConvolution(irs[0], filter.getKernel());
			}
			{
				Filter filter = Filter(Filter::filterType::BPF, 125, 250);
				irs[1] = FFTConvolution(irs[1], filter.getKernel());
			}
			{
				Filter filter = Filter(Filter::filterType::BPF, 250, 500);
				irs[2] = FFTConvolution(irs[2], filter.getKernel());
			}
			{
				Filter filter = Filter(Filter::filterType::BPF, 500, 1000);
				irs[3] = FFTConvolution(irs[3], filter.getKernel());
			}
			{
				Filter filter = Filter(Filter::filterType::BPF, 1000, 2000);
				irs[4] = FFTConvolution(irs[4], filter.getKernel());
			}
			{
				Filter filter = Filter(Filter::filterType::BPF, 2000, 20000);
				irs[5] = FFTConvolution(irs[5], filter.getKernel());
			}


			output.clear();
			output.resize(irs[0].size());
			for (int value = 0; value < output.size(); value++)
				output[value] = irs[0][value] + irs[1][value] + irs[2][value] + irs[3][value] + irs[4][value] + irs[5][value];
			//
			//{
			//	Filter filter = Filter(Filter::HPF, DSP_nTaps, samplingFrequency, 25);
			//	filter.convolveToSignal(output);
			//	WriteAudioFile({ filter.getIR() }, "Filter_hpf.wav", samplingFrequency);
			//}

			NormaliseSignal(output);
			WriteAudioFile({ output }, "ir.wav", samplingFrequency);

		}


	
		int loadPredictions(const std::string& predictionCsv, std::vector<SurfacePatch>& outPatches)
		{
			std::ifstream csvFile(predictionCsv);
			outPatches.clear();

			std::string line;
			if (csvFile.is_open()) {
				std::getline(csvFile, line);
				while (std::getline(csvFile, line)) {
					size_t pos = 0;
					SurfacePatch patch{};
					std::string token;
					int i = 0;
					while ((pos = line.find(',')) != std::string::npos) {
					token = line.substr(0, pos);
					line.erase(0, pos + 1);

					if (i == 0) patch.name = token;
					if (i == 1) patch.label = token;
					if (i == 2) patch.confidence = std::stof(token);
					if (i == 3) patch.coefficients[0] = std::stof(token);
					if (i == 4) patch.coefficients[1] = std::stof(token);
					if (i == 5) patch.coefficients[2] = std::stof(token);
					if (i == 6) patch.coefficients[3] = std::stof(token);
					if (i == 7) patch.coefficients[4] = std::stof(token);
					if (i == 8) patch.coefficients[5] = std::stof(token);

					i++;
					}
					outPatches.push_back(patch);
				}
				csvFile.close();
			}
			else {
				UNDA_ERROR("Could not open: " + predictionCsv);
				return -1;
			}
			return 0;
		}

	}
}