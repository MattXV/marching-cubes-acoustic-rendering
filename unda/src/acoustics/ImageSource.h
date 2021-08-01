#pragma once

#include "Acoustics.h"
#include "DSP.h"
#include "../utils/Maths.h"
#include "../utils/Settings.h"
#include "../utils/Utils.h"
#include <queue>
#include <mutex>

#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <array>
#include <stdexcept>
#include <functional>
#define _USE_MATH_DEFINES
#include <math.h>
#include <unordered_map>


#define ROUND(x) ((x) >= 0 ? (long)((x) + 0.5) : (long)((x) - 0.5))



namespace unda {
	namespace acoustics {
		template<typename T> inline const T sinc(T const& x) {
			return (x == 0) ? 1 : sin(x) / x;
			0 ? true : false;
		}

		class ImageSourceModel {
		public:
			ImageSourceModel(const std::array<double, 3>& _spaceDimensions, const std::array<double, 3>& _sourcePosition,
							 const std::array<double, 3>& _receiverPosition, std::array<std::array<double, 6>, 6>& _surfaceReflection, int _nSamples = 0, unsigned int order=2);
			~ImageSourceModel() = default;

			std::array<Signal, 6> getIRs() { return irs; }

			void dispatchCPUThreads();
			void updateParameters();


		private:
			// Variables

			// Acoustic parameters
			double meanFreePathEstimate = 0;
			double t_60 = 0;

			// X, Y and Z space imensions in metres
			long double totalSurface = 0.0;
			const std::array<double, 3>& spaceDimensions;
			std::array<std::array<double, 6>, 6>& surfaceReflection; // F_cubeFace x Coeff_frequencyBin 
			std::array<double, 3> sourcePosition;
			std::array<double, 3> receiverPosition;
			std::array<double, 2> microphoneAngle{ 0, 0 };

			// Impulse Response Data
			// Frequency-dependent RIRs - [125 - 250, 250 - 500, 500 - 1000, 1000 - 2000, 2000 - 4000, 6000 - 12000]
			std::array<Signal, 6> irs;
			Signal output;

			// Acoustic Volume variables
			unsigned int order = 2;
			double samplingFrequency = unda::sampleRate;
			const double timeStep = unda::maths::c / unda::sampleRate;
			int nSamples = 0;
			double source[3] { 0 };
			double listener[3] { 0 };
			double room[3] { 0 };

			void computeReflections(int x, int y, int z);

			// Thread workers
			std::vector<std::thread> workers;
			void computeTail();

			DISABLE_COPY_ASSIGN(ImageSourceModel)
		};


		class GPUImageSourceModel : public ImageSourceModel {
		public:
			GPUImageSourceModel();
			~GPUImageSourceModel() = default;
		private:
			DISABLE_COPY_ASSIGN(GPUImageSourceModel)
		};
	}
}

