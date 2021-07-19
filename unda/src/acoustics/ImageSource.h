#pragma once

#include "Acoustics.h"
#include "DSP.h"
#include "../utils/Maths.h"
#include "../utils/Settings.h"

#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <array>
#include <stdexcept>
#include <functional>
#include <math.h>
#include <iostream>
#include <unordered_map>
#include <fstream>


#define _USE_MATH_DEFINES

#define ROUND(x) ((x) >= 0 ? (long)((x) + 0.5) : (long)((x) - 0.5))

#ifndef M_PI 
	#define M_PI 3.14159265358979323846 
#endif


namespace unda {
	namespace acoustics {
		template<typename T> inline const T sinc(T const& x) {
			return (x == 0) ? 1 : sin(x) / x;
			0 ? true : false;
		}
		struct SurfacePatch {
			std::string name, label;
			float confidence;
			std::array<float, 6> coefficients;
		};

		class ImageSourceModel {
		public:
			ImageSourceModel(int _nThreads, const std::array<double, 3>& _spaceDimensions, const std::array<double, 3>& _sourcePosition,
							 const std::array<double, 3>& _receiverPosition, std::array<std::array<double, 6>, 6>& _surfaceReflection, int _nSamples = 0, unsigned int order=2);
			~ImageSourceModel();

			std::array<Signal, 6> getIRs() { return irs; }

			void setSourcePosition(const std::array<double, 3>& position) { sourcePosition = position; }
			void setReceiverPosition(const std::array<double, 3>& position) { receiverPosition = position; }
			void setSamplingFrequency(double newFs) { samplingFrequency = newFs; }
			void setNSamples(int newNSamples) { nSamples = newNSamples; }

			void generateIR();
			void doPatches(const std::vector<SurfacePatch>& patches, int cellsPerDimesion);

		private:

			// Variables
			unsigned int order = 2;
			double samplingFrequency;
			double speedOfSound;
			int nSamples = 0;
			int DSP_nTaps = 8192;

			// Acoustic parameter estimates
			double meanFreePathEstimate;
			double t_60;

			// X, Y and Z space imensions in metres
			long double totalSurface = 0.0;
			const std::array<double, 3>& spaceDimensions;
			std::array<std::array<double, 6>, 6>& surfaceReflection; // F_cubeFace x Coeff_frequencyBin 
			std::array<double, 3> sourcePosition;
			std::array<double, 3> receiverPosition;
			std::array<double, 2> microphoneAngle{ 0, 0 };
			std::mutex variablesMutex;

			// Data
			// Frequency-dependent RIRs - [125 - 250, 250 - 500, 500 - 1000, 1000 - 2000, 2000 - 4000, 6000 - 12000]
			std::array<Signal, 6> irs;
			Signal output;

			// Thread workers
			std::vector<std::thread> workers;
			int nThreads;
			void computeIRs(int threadId);
			void computeTail();

			ImageSourceModel(const ImageSourceModel&) = delete;
			ImageSourceModel& operator=(const ImageSourceModel&) = delete;
		};

		int loadPredictions(const std::string& predictionCsv, std::vector<SurfacePatch>& outPatches);
	}
}

