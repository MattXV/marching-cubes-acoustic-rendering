#pragma once

#include "../utils/Maths.h"
#include "../utils/Settings.h"

#include <thread>
#include <mutex>

#include <vector>
#include <array>
#include <stdexcept>
#include <functional>
//#include <cmath.h>
#include <math.h>
//#include <limits>
//#include <cstdlib>
#include <iostream>

#define _USE_MATH_DEFINES

#define ROUND(x) ((x) >= 0 ? (long)((x) + 0.5) : (long)((x) - 0.5))

#ifndef M_PI 
#define M_PI 3.14159265358979323846 
#endif


namespace unda {
	namespace acoustics {
		std::array<std::vector<double>, 6> GenerateRIR(double c, double fs, const std::vector<double>& receiver,
													   const std::vector<double>& source, const std::vector<double>& roomDimensions, std::vector<std::array<double, 6>>& beta_input,
													   const std::vector<double>& orientation, int nDimension, int nOrder, int nSamples, char microphone_type);

		template<typename T> inline const T sinc(T const& x) {
			return (x == 0) ? 1 : sin(x) / x;
			0 ? true : false;
		}


		class ImageSourceModel final {
		public:
			ImageSourceModel(int _nThreads, const std::array<double, 3>& _spaceDimensions, const std::array<double, 3>& _sourcePosition,
							 const std::array<double, 3>& _receiverPosition, const std::array<std::array<double, 6>, 6>& _surfaceReflection, int _nSamples = 0);
			~ImageSourceModel();

			std::array<std::vector<double>, 6> getIRs() { return irs; }

			void setSourcePosition(const std::array<double, 3>& position) { sourcePosition = position; }
			void setReceiverPosition(const std::array<double, 3>& position) { receiverPosition = position; }
			void setSamplingFrequency(double newFs) { samplingFrequency = newFs; }
			void setNSamples(int newNSamples) { nSamples = newNSamples; }

			void generateIRs();

		private:

			// Variables
			double samplingFrequency;
			double speedOfSound;
			int nSamples;
			// X, Y and Z space imensions in metres
			const std::array<double, 3>& spaceDimensions;
			const std::array<std::array<double, 6>, 6>& surfaceReflection; // F_cubeFace x Coeff_frequencyBin 
			std::array<double, 3> sourcePosition;
			std::array<double, 3> receiverPosition;
			std::array<double, 2> microphoneAngle{ 0, 0 };
			std::mutex variablesMutex;

			// Data
			// Frequency-dependent RIRs - [125 - 250, 250 - 500, 500 - 1000, 1000 - 2000, 2000 - 4000, 6000 - 12000]
			std::array<std::vector<double>, 6> irs;

			// Thread workers
			std::vector<std::thread> workers;
			int nThreads;
			void computeIRs(int threadId);

			ImageSourceModel(const ImageSourceModel&) = delete;
			ImageSourceModel& operator=(const ImageSourceModel&) = delete;
		};
	}
}

