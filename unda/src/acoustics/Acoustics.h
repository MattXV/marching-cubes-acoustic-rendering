#pragma once

//#include "../utils/Settings.h"
//#include "../utils/Maths.h"
//#include "../utils/Utils.h"
//#include "../rendering/RenderTools.h"
//#include "../utils/Settings.h"

#include <functional>
#include <vector>
#include <array>
#include <algorithm>



namespace unda {
	namespace acoustics {
		static inline double meanFreePath(double totalVolume, double totalSurface) { 
			return 4.0 * (totalVolume / totalSurface); 
		}
		static inline double alphaToBeta(double alpha) {
			return sqrt(1.0 - alpha);
		}

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
				std::array<double, 6> beta = alphaCoefficients;
				std::for_each(beta.begin(), beta.end(), [](double& coeff) { coeff = sqrt(1.0 - coeff); });
				return beta;
			}
		};
		namespace Materials {
			static Material floor = Material("Floor", { 0.087307, 0.08230, 0.144615, 0.20076, 0.24653, 0.26692 });
			static Material fabric = Material("Fabric", { 0.02999, 0.119999, 0.150000, 0.270000, 0.370000, 0.419999 });
			static Material painted = Material("Fabric", { 0.7999, 0.69999, 0.300999, 0.129999, 0.1929999, 0.1949999 });
			static Material carpet = Material("Carpet", { 0.01, 0.02, 0.06, 0.15000, 0.25, 0.449999 });
			
			static Material wallTreatments = Material("Wall_Treatments", { 0.35, 0.35, 0.25, 0.16, 0.25, 0.3568 });
			static Material officeFloor = Material("Wall_Treatments", { 0.37307, 0.39230, 0.444615, 0.20076, 0.24653, 0.26692 });
			static Material officeCeiling = Material("Wall_Treatments", { 0.47307, 0.4230, 0.644615, 0.50076, 0.24653, 0.256692 });
			
			static Material defaultMaterial = Material("Default", { 0.01, 0.01, 0.03, 0.02, 0.027, 0.027 });

			static Material ceramic = Material("Default", { 0.02, 0.02, 0.02, 0.020, 0.0327, 0.0527 });
			static Material concrete = Material("Default", { 0.02, 0.03, 0.032, 0.0310, 0.04127, 0.07127 });
			static Material smoothBricks = Material("Default", { 0.2, 0.2, 0.15, 0.11, 0.157, 0.36727 });

		}
		
	}
}