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

		std::array<std::vector<double>, 6> GenerateRIR(double c, double fs, const std::vector<double>& receiver,
			const std::vector<double>& source, const std::vector<double>& roomDimensions, std::vector<std::array<double, 6>>& beta_input,
			const std::vector<double>& orientation, int nDimension, int nOrder, int nSamples, char microphone_type) {
			// | Room Impulse Response Generator                                  |\n"
			// |                                                                  |\n"
			// | Computes the response of an acoustic source to one or more       |\n"
			// | microphones in a reverberant room using the image method [1,2].  |\n"
			// |                                                                  |\n"
			// | Author    : dr.ir. Emanuel Habets (ehabets@dereverberation.org)  |\n"
			// |                                                                  |\n"
			// | Version   : 2.1.20141124                                         |\n"
			// |                                                                  |\n"
			// | Copyright (C) 2003-2014 E.A.P. Habets, The Netherlands.          |\n"
			// |                                                                  |\n"
			// | [1] J.B. Allen and D.A. Berkley,                                 |\n"
			// |     Image method for efficiently simulating small-room acoustics,|\n"
			// |     Journal Acoustic Society of America,                         |\n"
			// |     65(4), April 1979, p 943.                                    |\n"
			// |                                                                  |\n"
			// | [2] P.M. Peterson,                                               |\n"
			// |     Simulating the response of multiple microphones to a single  |\n"
			// |     acoustic source in a reverberant room, Journal Acoustic      |\n"
			// |     Society of America, 80(5), November 1986.                    |\n"
			// --------------------------------------------------------------------\n\n"
			// function [h, beta_hat] = rir_generator(c, fs, r, s, L, beta, nsample,\n"
			//  mtype, order, dim, orientation, hp_filter);\n\n"
			// Input parameters:\n"
			//  c           : sound velocity in m/s.\n"
			//  fs          : sampling frequency in Hz.\n"
			//  r           : M x 3 array specifying the (x,y,z) coordinates of the\n"
			//                receiver(s) in m.\n"
			//  s           : 1 x 3 vector specifying the (x,y,z) coordinates of the\n"
			//                source in m.\n"
			//  L           : 1 x 3 vector specifying the room dimensions (x,y,z) in m.\n"
			// 
			// 	FEATURE ADDED: Frequency-dependent beta coefficients
			//  beta        : N_f x 6 vector specifying the reflection coefficients\n"
			//                [beta_x1 beta_x2 beta_y1 beta_y2 beta_z1 beta_z2] or\n"
			//                beta = reverberation time (T_60) in seconds.\n"
			//				  where N_f is the number of frequency bins.\n"
			// 
			//  nsample     : number of samples to calculate, default is T_60*fs.\n"
			//  mtype       : [omnidirectional, subcardioid, cardioid, hypercardioid,\n"
			//                bidirectional], default is omnidirectional.\n"
			//  order       : reflection order, default is -1, i.e. maximum order.\n"
			//  dim         : room dimension (2 or 3), default is 3.\n"
			//  orientation : direction in which the microphones are pointed, specified using\n"
			//                azimuth and elevation angles (in radians), default is [0 0].\n"
			//  hp_filter   : use 'false' to disable high-pass filter, the high-pass filter\n"
			//                is enabled by default.\n\n"
			// Output parameters:\n"
			//  h           : M x nsample matrix containing the calculated room impulse\n"
			//                response(s).\n"
			//  beta_hat    : In case a reverberation time is specified as an input parameter\n"
			//                the corresponding reflection coefficient is returned.\n\n");

			// Load parameters
			double       beta[6][6];
			double       angle[2];
			double       reverberation_time;

			if (beta_input.size() == 1) {
				throw std::invalid_argument("Invalid beta input!");

				double V = roomDimensions[0] * roomDimensions[1] * roomDimensions[2];
				double S = 2 * (roomDimensions[0] * roomDimensions[2] + 
						        roomDimensions[1] * roomDimensions[2] +
								roomDimensions[0] * roomDimensions[1]);

				double average_reverberation = 0.0;
			} else {
				for (int i = 0; i < 6; i++) {
					for (int bin = 0; bin < 6; bin++) {
						std::array<double, 6>& coefficients = beta_input[i];
						beta[i][bin] = coefficients[bin];
					}
				}
			}

			// 3D Microphone orientation (optional)
			if (orientation.size()) {
				angle[0] = orientation[0];
				angle[1] = orientation[1];
			}
			else {
				angle[0] = 0;
				angle[1] = 0;
			}
			if (nDimension != 3) {
				throw std::invalid_argument("Invalid number of dimensions!");
			}

			// Reflection order (optional)
			if (nOrder < -1)
			{
				throw std::invalid_argument("Invalid nOrder parameter!");
			}

			// Number of samples (optional)
			if (nSamples == -1) {
				if (beta_input.size() > 1) {
					double V = roomDimensions[0] * roomDimensions[1] * roomDimensions[2];

					double average_alpha = 0.0;
					for (int bin = 0; bin < 6; bin++) {
						double alpha = ((1 - pow(beta[0][bin], 2)) + (1 - pow(beta[1][bin], 2))) * roomDimensions[1] * roomDimensions[2] +
							((1 - pow(beta[2][bin], 2)) + (1 - pow(beta[3][bin], 2))) * roomDimensions[0] * roomDimensions[2] +
							((1 - pow(beta[4][bin], 2)) + (1 - pow(beta[5][bin], 2))) * roomDimensions[0] * roomDimensions[1];
						average_alpha = (alpha + average_alpha) / 2.0;
					}
					reverberation_time = 24 * log(10.0) * V / (c * average_alpha);
					if (reverberation_time < 0.128)
						reverberation_time = 0.128;
				}
				nSamples = (int)(reverberation_time * fs);
			}

			// Create output vector
			std::array<std::vector<double>, 6> impulseResponses;
			for (int bin = 0; bin < 6; bin++)
				impulseResponses[bin].resize(nSamples);

			// Temporary variables and constants (high-pass filter)
			const double W = 2 * M_PI * 100 / fs; // The cut-off frequency equals 100 Hz
			const double R1 = exp(-W);
			const double B1 = 2 * R1 * cos(W);
			const double B2 = -R1 * R1;
			const double A1 = -(1 + R1);
			double       X0;
			double*		 Y = new double[3];
			
			// Temporary variables and constants (image-method)
			const double Fc = 1; // The cut-off frequency equals fs/2 - Fc is the normalized cut-off frequency.
			const int    Tw = 2 * ROUND(0.004 * fs); // The width of the low-pass FIR equals 8 ms
			const double timeStep = c / fs;
			double*		 LPI = new double[Tw];

			double*		 s = new double[3];
			double*		 L = new double[3];
			double       Rm[3];
			double       Rp_plus_Rm[3];

			double		 reflections[3][8];  // multidimensional array N x 3; N -> octave bands
			double       fdist, dist;
			double       gain;
			int          startPosition;
			int          q, j, k;
			int          mx, my, mz;
			int          n;

			int          x, y, z;
			s[0] = source[0] / timeStep;
			s[1] = source[1] / timeStep;
			s[2] = source[2] / timeStep;
			L[0] = receiver[0] / timeStep;
			L[1] = receiver[1] / timeStep;
			L[2] = receiver[2] / timeStep;

			x = (int)ceil(nSamples / (2 * L[0]));
			y = (int)ceil(nSamples / (2 * L[1]));
			z = (int)ceil(nSamples / (2 * L[2]));

			// Generate room impulse response
			for (mx = -x; mx <= x; mx++)
			{
				Rm[0] = 2 * (double)mx * L[0];

				for (my = -y; my <= y; my++)
				{
					Rm[1] = 2 * (double)my * L[1];

					for (mz = -z; mz <= z; mz++)
					{
						Rm[2] = 2 * (double)mz * L[2];

						for (q = 0; q <= 1; q++)
						{
							Rp_plus_Rm[0] = (1 - 2 * (double)q) * s[0] - L[0] + Rm[0];

							// Frequency-dependent relfection calculation :D
							for (int bin = 0; bin < 6; bin++) {
								reflections[0][bin] = pow(beta[0][bin], abs(mx - q)) * pow(beta[1][bin], abs(mx));
							}

							for (j = 0; j <= 1; j++)
							{
								Rp_plus_Rm[1] = (1 - 2 * (double)j) * s[1] - L[1] + Rm[1];
								for (int bin = 0; bin < 6; bin++) {
									reflections[1][bin] = pow(beta[2][bin], std::abs(my - j)) * pow(beta[3][bin], std::abs(my));
								}

								for (k = 0; k <= 1; k++)
								{
									Rp_plus_Rm[2] = (1 - 2 * (double)k) * s[2] - L[2] + Rm[2];

									for (int bin = 0; bin < 6; bin++) {
										reflections[2][bin] = pow(beta[4][bin], std::abs(mz - k)) * pow(beta[5][bin], std::abs(mz));
									}

									dist = sqrt(pow(Rp_plus_Rm[0], 2) + pow(Rp_plus_Rm[1], 2) + pow(Rp_plus_Rm[2], 2));

									if (std::abs(2 * mx - q) + std::abs(2 * my - j) + std::abs(2 * mz - k) <= nOrder || nOrder == -1)
									{
										fdist = floor(dist);
										if (fdist < nSamples)
										{
											std::array<double, 6> gains;

											double microphone = sim_microphone(Rp_plus_Rm[0], Rp_plus_Rm[1], Rp_plus_Rm[2], angle, microphone_type);
											for (int bin = 0; bin < 6; bin++) {
												gain = (microphone * reflections[0][bin] * reflections[1][bin] * reflections[2][bin]) / (4 * M_PI * dist * timeStep);
												gains[bin] = (double)gain;
											}

											for (n = 0; n < Tw; n++)
												LPI[n] = 0.5 * (1 - cos(2 * M_PI * (((double)n + 1 - (dist - fdist)) / Tw))) * Fc * sinc(M_PI * Fc * ((double)n + 1 - (dist - fdist) - (Tw / 2)));

											startPosition = (int)fdist - (Tw / 2) + 1;
											for (n = 0; n < Tw; n++)
												for (int bin = 0; bin < 6; bin++)
													if (startPosition + n >= 0 && startPosition + n < nSamples)
														impulseResponses[bin][startPosition + (size_t)n] += gains[bin] * LPI[n];
										}
									}
								}
							}
						}
					}
				}
			}

	
			delete[] Y;
			delete[] LPI;
			delete[] s;
			delete[] L;

			return impulseResponses;
		}


		ImageSourceModel::ImageSourceModel(int _nThreads, const std::array<double, 3>& _spaceDimensions, const std::array<double, 3>& _sourcePosition, const std::array<double, 3>& _receiverPosition, const std::array<std::array<double, 6>, 6>& _surfaceReflection, int _nSamples)
			: spaceDimensions(_spaceDimensions)
			, sourcePosition(_sourcePosition)
			, receiverPosition(_receiverPosition)
			, surfaceReflection(_surfaceReflection)
		{
			if (nThreads > 32) { throw std::invalid_argument("Invalind number of threads!"); return; }
			nThreads = _nThreads;
			samplingFrequency = unda::sampleRate;
			speedOfSound = unda::maths::c;
			if (_nSamples < 1) {
				double volume = spaceDimensions[0] * spaceDimensions[1] * spaceDimensions[2];
				double totalAlpha = 0.0;
				for (int bin = 0; bin < 6; bin++) {
					// Using Sabine's equation to determine space reverberation if n_samples is not known.
					double alpha =
						spaceDimensions[0] * spaceDimensions[2] * surfaceReflection[0][bin] + // floor
						spaceDimensions[0] * spaceDimensions[2] * surfaceReflection[1][bin] + // celing
						spaceDimensions[1] * spaceDimensions[0] * surfaceReflection[2][bin] + // back wall
						spaceDimensions[1] * spaceDimensions[0] * surfaceReflection[3][bin] + // front wall
						spaceDimensions[1] * spaceDimensions[2] * surfaceReflection[4][bin] + // left wall
						spaceDimensions[1] * spaceDimensions[2] * surfaceReflection[5][bin];  // right wall
					totalAlpha += alpha;
				}
				totalAlpha /= 6.0;
				double t_60 = 0.161 * (volume / totalAlpha);
				nSamples = (int)round(t_60 * samplingFrequency);
			}
			else {
				nSamples = _nSamples;
			}
		}

		ImageSourceModel::~ImageSourceModel()
		{

		}

		void ImageSourceModel::generateIRs()
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

			double* angle = new double[2];
			angle[0] = microphoneAngle[0];
			angle[1] = microphoneAngle[1];
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

	}

}