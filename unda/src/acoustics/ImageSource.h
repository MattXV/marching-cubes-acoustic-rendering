#pragma once

#include <vector>
#include <array>
#include <stdexcept>
#include <functional>
//#include <cmath.h>
#include <math.h>
//#include <limits>
//#include <cstdlib>
//#include <iostream>

#define _USE_MATH_DEFINES

#define ROUND(x) ((x) >= 0 ? (long)((x) + 0.5) : (long)((x) - 0.5))

#ifndef M_PI 
#define M_PI 3.14159265358979323846 
#endif


namespace unda {
	namespace acoustics {
		std::vector<std::vector<double>> GenerateRIR(double c, double fs, const std::vector<std::vector<double>>& receivers,
			const std::vector<double>& ss, const std::vector<double>& LL, std::vector<std::array<double, 6>>& beta_input,
			const std::vector<double>& orientation, int isHighPassFilter, int nDimension, int nOrder, int nSamples, char microphone_type);

		template<typename T> inline const T sinc(T const& x) {
			return (x == 0) ? 1 : sin(x) / x;
		}

	}
}

