#pragma once

#include "../utils/Settings.h"
#include "../utils/Utils.h"
#include "../utils/Maths.h"
#include <sndfile.h>
#include <FFTConvolver.h>
#include <vector>




namespace unda {
	typedef std::vector<double> Signal;

	void NormaliseSignal(Signal& audioSamples);
	Signal NormaliseSignal(const Signal& audioSamples);
	int WriteAudioFile(const std::vector<std::vector<double>>& audioChannels, const std::string& filePath, double samplingFrequency = unda::sampleRate);

	Signal AllPassFilter(std::vector<double>& audioSamples, float delayMilliseconds = 90.0f, float decay = 0.131f, double sampleRate = unda::sampleRate);
	Signal CombFilter(std::vector<double>& audioSamples, float delayMilliseconds = 90.0f, float decay = 0.131f, double sampleRate = unda::sampleRate);
	Signal SchroederReverb(const Signal& input, float delay, float decay);


	template<typename T>
	std::vector<T>	Convolve(std::vector<T> const& f, std::vector<T> const& g) {
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


	class Filter {
	public:
		enum filterType { LPF, HPF, BPF, COMB, ALLPASS };
		Filter(filterType filt_t, int num_taps, double Fs, double Fx);
		Filter(filterType filt_t, int num_taps, double Fs, double Fl, double Fu);
		~Filter();

		void convolveToSignal(Signal& signal);
		Signal convolveToSignal(const Signal& signal);
		int getError() { return errorFlag; };

		template<typename T>
		void getTaps(T* out_taps) {
			if (errorFlag != 0) return;
			for (int i = 0; i < nTaps; i++) out_taps[i] = T(taps[i]);
		}

	private:
		double omega;					// Cutoff frequency
		double omegaLower, omegaUpper;	// Lower and Upper Cutoff for BandPass types
		double fs;
		filterType type;
		size_t nTaps;
		Signal taps;
		int errorFlag;
		std::unique_ptr<fftconvolver::FFTConvolver> convolver;

		void designLPF();
		void designHPF();
		void designBPF();
		void designAllPass();

		void setError(int x) { errorFlag = x; UNDA_ERROR("Error in Filter operations!"); }
		DISABLE_COPY_ASSIGN(Filter);
	};
}