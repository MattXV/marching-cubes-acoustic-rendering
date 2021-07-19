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

		inline bool SameSign(float a, float b) {
			return a * b >= 0.0f;
		}
	}
		template<typename T>
		static inline void integerAddSwap(T* a, T* b) {
			*a = *a + *b;
			*b = *a - *b;
			*a = *a - *b;
		}

		static unsigned int roundUpToNextPowerOfTwo(unsigned int x)
		{
			x--;
			x |= x >> 1;  // handle  2 bit numbers
			x |= x >> 2;  // handle  4 bit numbers
			x |= x >> 4;  // handle  8 bit numbers
			x |= x >> 8;  // handle 16 bit numbers
			x |= x >> 16; // handle 32 bit numbers
			x++;

			return x;
		}

}