#pragma once

#include "../utils/Settings.h"
#include "../utils/Utils.h"
#include "../utils/Maths.h"
#include <sndfile.h>
#include <vector>
#include <pffft.h>
#include <string>


namespace unda {
	typedef float Sample;
	typedef std::vector<Sample> Signal;

	void NormaliseSignal(Signal& audioSamples);
	Signal NormaliseSignal(const Signal& audioSamples);
	int WriteAudioFile(const std::vector<Signal>& audioChannels, const std::string& filePath, double samplingFrequency = unda::sampleRate);
	Signal ReadAudioFileIntoMono(const std::string& filePath);
	void ZeroCrossingFadeInOut(Signal& signal);

	template<typename T>
	std::vector<T> TimeDomainConvolution(std::vector<T> const& f, std::vector<T> const& g) {
		// For plebs and 'slow' people like me.
		int const nf = (int)f.size();
		int const ng = (int)g.size();
		int const n = nf + ng - 1;
		std::vector<T> out(n, T());
		for (auto i(0); i < n; ++i) {
			int const jmn = (i >= ng - 1) ? i - (ng - 1) : 0;
			int const jmx = (i < nf - 1) ? i : nf - 1;
			for (auto j(jmn); j <= jmx; ++j) {
				out[i] += (f[j] * g[i - j]);
			}
		}
		return out;
	}
	Signal FFTConvolution(const Signal& signal, const Signal& kernel);


	class Filter {
	public:
		enum class filterType { LPF, HPF, BPF };
		Filter(filterType filt_t, float cutoff_hz);
		Filter(filterType filt_t, float lower_cutoff_hz, float upper_cutoff_hz);
		~Filter() = default;

		inline void writeFilterToFile(std::string filename) { Signal out = Signal(h.begin(), h.end()); WriteAudioFile({ out }, filename, (double)fs); }
		const Signal& getKernel() { return h; }

	private:
		// Parameters
		float lower_fc = 0, upper_fc = 0, fs = (float)unda::sampleRate;
		unsigned int M = (unsigned int)pow(2, 13);
		
		// Kernel
		Signal h;

		DISABLE_COPY_ASSIGN(Filter);
	};
}