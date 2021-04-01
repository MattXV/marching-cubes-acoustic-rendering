#pragma once

namespace unda {
	void CombFilter();
	void generateTail(float* input, float* output, size_t nSamples, size_t nAllPassFilters);
}