#pragma once

#include "../rendering/RenderTools.h"
#include "../utils/Settings.h"
#include "ImageSource.h"
#include <vector>
#include <math.h>


namespace unda {

	class AcousticSpace {
	public:
		AcousticSpace() : spaceDimensions({ 0.0, 0.0, 0.0 }) { }
		~AcousticSpace() = default;

		std::vector<std::vector<double>> generateRIR(const std::vector<std::vector<double>>& listeners, 
			const std::vector<double>& source,
			int nSamples);

		void setSpaceDimensions(const std::vector<double>& dimensions) { spaceDimensions = dimensions; }


	private:
		Listener* listener = nullptr;
		std::vector<double> spaceDimensions;
		std::vector<double> betaCoefficients = 
		{
			0.8, // left wall
			0.8, // right wall
			0.8, // floor
			0.8, // ceiling
			0.8, // front wall
			0.8  // back wall
		};
		double speedOfSound = 343;
	};
}