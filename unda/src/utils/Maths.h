#pragma once
#include <math.h>
#include <glm/ext/scalar_constants.hpp>

namespace unda {
	namespace maths {

		static constexpr double sinc(double x) {
			if (x > -1.0e-5 && x < 1.0e-5) return 1.0;
			return sin(x) / x;
		}
		static constexpr double pi = glm::pi<double>();
		static double c = 343.0;						// Speed of sound

	}

}