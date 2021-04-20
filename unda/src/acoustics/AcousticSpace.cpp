#include "AcousticSpace.h"

namespace unda {
	namespace acoustics {

		std::array<std::vector<double>, 6> unda::acoustics::AcousticSpace::generateRIR(const std::vector<double>& listener,
			const std::vector<double>& source, int nSamples)
		{

			std::vector<std::array<double, 6>> totalReflection;
			totalReflection.push_back(Materials::carpet.getBetaCoefficients());
			totalReflection.push_back(Materials::wallTreatments.getBetaCoefficients());
			totalReflection.push_back(Materials::wallTreatments.getBetaCoefficients());
			totalReflection.push_back(Materials::floor.getBetaCoefficients());
			totalReflection.push_back(Materials::wallTreatments.getBetaCoefficients());
			totalReflection.push_back(Materials::wallTreatments.getBetaCoefficients());

			//std::for_each(betaCoefficients.begin(), betaCoefficients.end(), [](std::array<double, 6>& coeffs) {
			//	std::for_each(coeffs.begin(), coeffs.end(), [](double& value) {	std::cout << value << " "; });
			//	std::cout << std::endl; }
			//);

			for (int i = 0; i < totalReflection.size(); i++) {
				std::array<double, 6> coeffs = totalReflection[i];
				for (int j = 0; j < 6; j++) {
					std::cout << coeffs[j] << " ";
				}
				std::cout << std::endl;
			}

			std::array<std::vector<double>, 6> rirs = GenerateRIR(unda::maths::c, unda::sampleRate, listener, source, spaceDimensions, totalReflection, { 0, 0 }, 3, -1, nSamples, 'o');
			

			// Normalise
			//for (std::vector<double>& rirVector : rirs) {
			//	double maxValue = 0.0, tempValue;
			//	for (double& value : rirVector) {
			//		tempValue = abs(value);
			//		if (tempValue > maxValue) maxValue = tempValue;
			//	}
			//	for (size_t i = 0; i < rirVector.size(); i++) {
			//		rirVector[i] /= maxValue;
			//	}
			//}
			return rirs;
		}

		void unda::acoustics::convertAlphaToBeta(std::vector<std::array<double, 6>>& alphaCoefficients)
		{
			std::function alphaToBeta = [](std::array<double, 6>& coefficients) {
				std::for_each(coefficients.begin(), coefficients.end(), [](double& value) { value = sqrt(1.0 - value); });
			};
			std::for_each(alphaCoefficients.begin(), alphaCoefficients.end(), alphaToBeta);
		}

		void unda::acoustics::convertBetaToAlpha(std::vector<std::array<double, 6>>& betaCoefficients)
		{
			std::function betaToAlpha = [](std::array<double, 6>& coefficients) {
				std::for_each(coefficients.begin(), coefficients.end(), [](double& value) { value = 1.0 - pow(value, 2); });
			};
			std::for_each(betaCoefficients.begin(), betaCoefficients.end(), betaToAlpha);
		}
	}
}