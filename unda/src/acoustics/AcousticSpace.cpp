#include "AcousticSpace.h"


std::vector<std::vector<double>> unda::AcousticSpace::generateRIR(const std::vector<std::vector<double>>& listeners,
	const std::vector<double>& source, int nSamples)
{
	std::vector<std::vector<double>> rirs = gen_rir(speedOfSound, unda::sampleRate, listeners, source, spaceDimensions, betaCoefficients, { 0, 0 }, 1, 3, -1, nSamples);

	// Normalise
	for (std::vector<double>& rirVector : rirs) {
		double maxValue = 0.0, tempValue;
		for (double& value : rirVector) {
			tempValue = abs(value);
			if (tempValue > maxValue) maxValue = tempValue;
		}
		for (size_t i = 0; i < rirVector.size(); i++) {
			rirVector[i] /= maxValue;
		}
	}
	return rirs;
}
