#include "AcousticSpace.h"

namespace unda {
	namespace acoustics {

		std::vector<std::vector<double>> unda::acoustics::AcousticSpace::generateRIR(const std::vector<std::vector<double>>& listeners,
			const std::vector<double>& source, int nSamples)
		{
			std::vector<std::array<double, 6>> betaCoefficients = alphaCoefficients;
			std::for_each(betaCoefficients.begin(), betaCoefficients.end(), [](std::array<double, 6>& coeffs) {
				std::for_each(coeffs.begin(), coeffs.end(), [](double& value) {	std::cout << value << " "; });
				std::cout << std::endl; }
			);

			std::cout << "--------" << std::endl;
			convertAlphaToBeta(betaCoefficients);

			std::for_each(betaCoefficients.begin(), betaCoefficients.end(), [](std::array<double, 6>& coeffs) {
				std::for_each(coeffs.begin(), coeffs.end(), [](double& value) {	std::cout << value << " "; });
				std::cout << std::endl; }
			);


			std::vector<std::vector<double>> rirs = GenerateRIR(speedOfSound, unda::sampleRate, listeners, source, spaceDimensions, betaCoefficients, { 0, 0 }, 1, 3, -1, nSamples, 'o');

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

		void unda::acoustics::convertAlphaToBeta(std::vector<std::array<double, 6>>& alphaCoefficients)
		{
			std::function alphaToBeta = [](std::array<double, 6>& coefficients) {
				std::for_each(coefficients.begin(), coefficients.end(), [](double& value) { std::cout << "original: " << value << std::endl; value = sqrt(1.0 - value);  std::cout << "modified: " << value << std::endl; });
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