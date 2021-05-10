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



		ImageSourceModel::ImageSourceModel(int _nThreads, const std::array<double, 3>& _spaceDimensions, const std::array<double, 3>& _sourcePosition, const std::array<double, 3>& _receiverPosition, std::array<std::array<double, 6>, 6>& _surfaceReflection, int _nSamples)
			: spaceDimensions(_spaceDimensions)
			, sourcePosition(_sourcePosition)
			, receiverPosition(_receiverPosition)
			, surfaceReflection(_surfaceReflection)
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
			for (int bin = 0; bin < 6; bin++) {
				// Using Sabine's equation to determine space reverberation if n_samples is not known.
				double alpha =
					floorSurface * surfaceReflection[0][bin] + // floor
					ceilingSurface * surfaceReflection[1][bin] + // celing
					backWallSurface * surfaceReflection[2][bin] + // back wall
					frontWallSurface * surfaceReflection[3][bin] + // front wall
					leftWallSurface * surfaceReflection[4][bin] + // left wall
					rightWallSurface * surfaceReflection[5][bin];  // right wall
				totalAlpha += alpha;
			}
			totalAlpha /= 6.0;
			t_60 = 0.161 * (volume / totalAlpha);
			totalSurface = floorSurface + ceilingSurface + backWallSurface + frontWallSurface + leftWallSurface + rightWallSurface;
			meanFreePathEstimate = meanFreePath(volume, (double)totalSurface);


			if (_nSamples < 1) {
				nSamples = (int)round(t_60 * samplingFrequency);
			}
			else {
				nSamples = _nSamples;
			}
		}

		ImageSourceModel::~ImageSourceModel()
		{

		}

		void ImageSourceModel::generateIR()
		{

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
		
			computeTail();
		}

		void ImageSourceModel::doPatches(const std::vector<SurfacePatch>& patches, int cellsPerDimesion)
		{
			long double patchContribution = totalSurface / std::pow((long double)cellsPerDimesion, 3);
			for (SurfacePatch patch : patches) {
				
				if (patch.name.find("bottomFace") != std::string::npos) {

					for (int i = 0; i < 6; i++) {
						long double patchCoefficient = (long double)patch.coefficients[i];
						patchCoefficient = (long double)alphaToBeta(patchCoefficient);
						surfaceReflection[0][i] = std::lerp((long double)surfaceReflection[0][i], patchCoefficient, patchContribution);
					}
				}

				if (patch.name.find("topFace") != std::string::npos) {

					for (int i = 0; i < 6; i++) {
						long double patchCoefficient = (long double)patch.coefficients[i];
						patchCoefficient = (long double)alphaToBeta(patchCoefficient);
						surfaceReflection[1][i] = std::lerp((long double)surfaceReflection[1][i], patchCoefficient, patchContribution);
					}
				}

				if (patch.name.find("backFace") != std::string::npos) {

					for (int i = 0; i < 6; i++) {
						long double patchCoefficient = (long double)patch.coefficients[i];
						patchCoefficient = (long double)alphaToBeta(patchCoefficient);
						surfaceReflection[2][i] = std::lerp((long double)surfaceReflection[2][i], patchCoefficient, patchContribution);
					}
				}
				if (patch.name.find("frontFace") != std::string::npos) {

					for (int i = 0; i < 6; i++) {
						long double patchCoefficient = (long double)patch.coefficients[i];
						patchCoefficient = (long double)alphaToBeta(patchCoefficient);
						surfaceReflection[3][i] = std::lerp((long double)surfaceReflection[3][i], patchCoefficient, patchContribution);
					}
				}
				if (patch.name.find("leftFace") != std::string::npos) {

					for (int i = 0; i < 6; i++) {
						long double patchCoefficient = (long double)patch.coefficients[i];
						patchCoefficient = (long double)alphaToBeta(patchCoefficient);
						surfaceReflection[4][i] = std::lerp((long double)surfaceReflection[4][i], patchCoefficient, patchContribution);
					}
				}

				if (patch.name.find("rightFace") != std::string::npos) {

					for (int i = 0; i < 6; i++) {
						long double patchCoefficient = (long double)patch.coefficients[i];
						patchCoefficient = (long double)alphaToBeta(patchCoefficient);
						surfaceReflection[5][i] = std::lerp((long double)surfaceReflection[5][i], patchCoefficient, patchContribution);
					}
				}
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

			double		 s[3];
			double		 L[3];
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

			s[0] = sourcePosition[0] / timeStep;
			s[1] = sourcePosition[1] / timeStep;
			s[2] = sourcePosition[2] / timeStep;
			L[0] = receiverPosition[0] / timeStep;
			L[1] = receiverPosition[1] / timeStep;
			L[2] = receiverPosition[2] / timeStep;


			// Image Source Model dimensions:
			// 
			// number of points computed along the Y axis
			int points_x = (int)ceil(nSamples / (2.0 * L[0]));
			int points_y = (int)ceil(nSamples / (2.0 * L[1]));
			int points_z = (int)ceil(nSamples / (2.0 * L[2]));
			//
			// Each thread is assigned with a slice of the cube representing the space
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
				Rm[0] = 2 * (double)mx * L[0];

				for (int my = startIndex; my < endIndex; my++)
				{
					Rm[1] = 2 * (double)my * L[1];

					for (int mz = -points_z; mz <= points_z; mz++)
					{
						Rm[2] = 2 * (double)mz * L[2];

						for (int q = 0; q <= 1; q++)
						{
							Rp_plus_Rm[0] = (1 - 2 * (double)q) * s[0] - L[0] + Rm[0];

							// Frequency-dependent relfection calculation :D
							for (int bin = 0; bin < 6; bin++) {
								reflections[0][bin] = pow(surfaceReflection[0][bin], abs(mx - q)) * pow(surfaceReflection[1][bin], abs(mx));
							}

							for (int j = 0; j <= 1; j++)
							{
								Rp_plus_Rm[1] = (1 - 2 * (double)j) * s[1] - L[1] + Rm[1];
								for (int bin = 0; bin < 6; bin++) {
									reflections[1][bin] = pow(surfaceReflection[2][bin], std::abs(my - j)) * pow(surfaceReflection[3][bin], std::abs(my));
								}

								for (int k = 0; k <= 1; k++)
								{
									Rp_plus_Rm[2] = (1 - 2 * (double)k) * s[2] - L[2] + Rm[2];

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
													irs[bin][startPosition + (size_t)n] += gains[bin] *LPI[n];
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
			output.clear();
				output.resize(irs[0].size());
			{
				Filter filter = Filter(Filter::BPF, DSP_nTaps, samplingFrequency, 20, 125);
				filter.convolveToSignal(irs[0]);
			}
			{
				Filter filter = Filter(Filter::BPF, DSP_nTaps, samplingFrequency, 125, 250);
				filter.convolveToSignal(irs[1]);
			}
			{
				Filter filter = Filter(Filter::BPF, DSP_nTaps, samplingFrequency, 250, 500);
				filter.convolveToSignal(irs[2]);
			}
			{
				Filter filter = Filter(Filter::BPF, DSP_nTaps, samplingFrequency, 500, 1000);
				filter.convolveToSignal(irs[3]);
			}
			{
				Filter filter = Filter(Filter::BPF, DSP_nTaps, samplingFrequency, 1000, 2000);
				filter.convolveToSignal(irs[4]);
			}
			{
				Filter filter = Filter(Filter::BPF, DSP_nTaps, samplingFrequency, 2000, 20000);
				filter.convolveToSignal(irs[5]);
			}

			for (int value = 0; value < output.size(); value++) {
				output[value] = irs[0][value] + irs[1][value] + irs[2][value] + irs[3][value] + irs[4][value] + irs[5][value];
			}

			NormaliseSignal(output);
			WriteAudioFile({ output }, "ir.wav", samplingFrequency);

			// Apply Schroeder reverb
			float delay = 950.9f;
			float decay = 0.9f;

			std::vector<double> comb1 = NormaliseSignal(CombFilter(output, delay, decay));
			std::vector<double> comb2 = NormaliseSignal(CombFilter(output, delay - 11.73f, decay - 0.131f));
			std::vector<double> comb3 = NormaliseSignal(CombFilter(output, delay + 19.31f, decay - 0.274f));
			std::vector<double> comb4 = NormaliseSignal(CombFilter(output, delay - 7.97f, decay - 0.31f));
			output.clear();
			output.resize(comb1.size());
			for (int i = 0; i < comb1.size(); i++) {
				output[i] = comb1[i] + comb2[i] + comb3[i] + comb4[i];
			}
			output = AllPassFilter(output);
			output = AllPassFilter(output);

			NormaliseSignal(output);
			WriteAudioFile({ output }, "ir_reverb.wav", samplingFrequency);
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