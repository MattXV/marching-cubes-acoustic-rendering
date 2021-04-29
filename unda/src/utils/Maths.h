#pragma once
#include <math.h>
#include <vector>
#include <assert.h>
#include <glm/ext/scalar_constants.hpp>

namespace unda {
	namespace maths {

		static constexpr double sinc(double x) {
			if (x > -1.0e-5 && x < 1.0e-5) return 1.0;
			return sin(x) / x;
		}
		static constexpr double pi = glm::pi<double>();
		static double c = 343.0;						// Speed of sound

		template<typename T>
		static inline float dot(const std::vector<T>& a, const std::vector<T>& b) {
			float sum = 0.0f;
			assert(a.size() == b.size());
			for (int i = 0; i < a.size(); i++) 
				sum += a[i] * b[i];
			return sum;
		}
	}

	template<typename T>
	static inline void integerAddSwap(T* a, T* b) {
		*a = *a + *b;
		*b = *a - *b;
		*a = *a - *b;
	}
}