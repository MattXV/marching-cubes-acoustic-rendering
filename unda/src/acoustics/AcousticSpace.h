#pragma once

#include "../utils/Settings.h"
#include "../utils/Maths.h"
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
		class Material {
		public:
			Material(const std::string& _label, const std::array<double, 6>& _alphaCoefficients)
				: label(_label)
				, alphaCoefficients(_alphaCoefficients)
			{}
			Material() = default;
			std::string label;
			std::array<double, 6> alphaCoefficients;
			inline std::array<double, 6> getBetaCoefficients() {
				std::function convertToBeta = [](double& coeff) { coeff = sqrt(1.0 - coeff); };
				std::array<double, 6> beta = alphaCoefficients;
				std::for_each(beta.begin(), beta.end(), convertToBeta);
				return beta;
			}
		};
		namespace Materials {
			static Material floor = Material("Floor", { 0.087307, 0.08230, 0.144615, 0.20076, 0.24653, 0.26692 });
			static Material fabric = Material("Fabric", { 0.02999, 0.119999, 0.150000, 0.270000, 0.370000, 0.419999 });
			static Material painted = Material("Fabric", { 0.019999, 0.019999, 0.019999, 0.019999, 0.019999, 0.019999 });
			static Material carpet = Material("Carpet", { 0.01, 0.02, 0.06, 0.15000, 0.25, 0.449999 });
			static Material wallTreatments = Material("Wall_Treatments", { 0.05, 0.06, 0.39, 0.63, 0.70, 0.73 });
		}
		

		class AcousticSpace {
		public:
			AcousticSpace() : spaceDimensions({ 0.0, 0.0, 0.0 }) { }
			~AcousticSpace() = default;

			std::array<std::vector<double>, 6> generateRIR(const std::vector<double>& listener,
				const std::vector<double>& source, int nSamples);

			void setSpaceDimensions(const std::vector<double>& dimensions) { spaceDimensions = dimensions; }


		private:
			Listener* listener = nullptr;
			std::vector<double> spaceDimensions;
			//std::vector<std::array<double, 6>> alphaCoefficients =
			//{
			//	{ 0.08730, 0.08230, 0.1446, 0.2007, 0.2465, 0.2669 }, // left wall
			//	{ 0.01999, 0.01999, 0.01999, 0.01999, 0.01999, 0.01999 }, // right wall
			//	{ 0.0873, 0.082, 0.144, 0.2007, 0.2465, 0.2669 }, // floor
			//	{ 0.0873, 0.08230, 0.1446, 0.20076, 0.2465, 0.2669 }, // ceiling
			//	{ 0.01999, 0.01999, 0.01999, 0.01999, 0.01999, 0.01999 }, // front wall
			//	{ 0.0873, 0.0823, 0.1446, 0.20076, 0.2465, 0.2669 }  // back wall
			//};

			double speedOfSound = 343;
		};

		void convertAlphaToBeta(std::vector<std::array<double, 6>>& alphaCoefficients);
		void convertBetaToAlpha(std::vector<std::array<double, 6>>& betaCoefficients);
	}


}