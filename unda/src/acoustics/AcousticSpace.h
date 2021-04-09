#pragma once

#include "../rendering/RenderTools.h"
#include "../utils/Settings.h"
#include "ImageSource.h"
#include <functional>
#include <vector>
#include <array>
#include <algorithm>
#include <math.h>


namespace unda {
	namespace acoustics {
		class AcousticSpace {
		public:
			AcousticSpace() : spaceDimensions({ 0.0, 0.0, 0.0 }) { }
			~AcousticSpace() = default;

			std::vector<std::vector<double>> generateRIR(const std::vector<std::vector<double>>& listeners,
				const std::vector<double>& source,
				int nSamples);

			void setSpaceDimensions(const std::vector<double>& dimensions) { spaceDimensions = dimensions; }


		private:
			Listener* listener = nullptr;
			std::vector<double> spaceDimensions;
			std::vector<std::array<double, 6>> alphaCoefficients =
			{
				{ 0.08730, 0.08230, 0.1446, 0.2007, 0.2465, 0.2669 }, // left wall
				{ 0.01999, 0.01999, 0.01999, 0.01999, 0.01999, 0.01999 }, // right wall
				{ 0.0873, 0.082, 0.144, 0.2007, 0.2465, 0.2669 }, // floor
				{ 0.0873, 0.08230, 0.1446, 0.20076, 0.2465, 0.2669 }, // ceiling
				{ 0.01999, 0.01999, 0.01999, 0.01999, 0.01999, 0.01999 }, // front wall
				{ 0.0873, 0.0823, 0.1446, 0.20076, 0.2465, 0.2669 }  // back wall
			};

			double speedOfSound = 343;
		};

		void convertAlphaToBeta(std::vector<std::array<double, 6>>& alphaCoefficients);
		void convertBetaToAlpha(std::vector<std::array<double, 6>>& betaCoefficients);
	}
}