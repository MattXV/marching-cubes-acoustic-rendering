#include "DSP.h"



namespace unda {
	void unda::NormaliseSignal(std::vector<double>& audioSamples)
	{
		double value = 0.0, tempValue;
		for (double& sample : audioSamples) {
			tempValue = abs(sample);
			if (tempValue > value) value = tempValue;
		}
		for (double& sample : audioSamples) {
			sample = (sample / value);
		}
	}

	std::vector<double> unda::NormaliseSignal(const std::vector<double>& audioSamples)
	{
		std::vector<double> output;
		double value = 0.0, tempValue;
		for (const double& sample : audioSamples) {
			tempValue = abs(sample);
			if (tempValue > value) value = tempValue;
		}
		for (double sample : audioSamples)
			output.push_back(sample / value);
		return output;
	}





	std::vector<double> AllPassFilter(std::vector<double>& audioSamples, float delayMilliseconds, float decay, double samplingFrequency)
	{
		size_t samplesLength = audioSamples.size();
		size_t delaySamples = (size_t)std::round((double)delayMilliseconds / (samplingFrequency / 1000.0));
		std::vector<double> allPassTaps;
		allPassTaps.resize(samplesLength);

		for (int i = 0; i < samplesLength; i++) {
			allPassTaps[i] = audioSamples[i];
			if (((double)i - (double)delaySamples) > 0) {
				allPassTaps[i] += -decay * allPassTaps[i - delaySamples];
			}
			if (((double)i - (double)delaySamples) >= 1)
				allPassTaps[i] += decay * allPassTaps[i - delaySamples];
		}

		return allPassTaps;
	}

	std::vector<double> CombFilter(std::vector<double>& audioSamples, float delayMilliseconds, float decay, double samplingFrequency)
	{
		size_t samplesLength = audioSamples.size();
		size_t delaySamples = (size_t)std::round((double)delayMilliseconds / (samplingFrequency / 1000.0));
		std::vector<double> allPassTaps{audioSamples};

		for (int i = 0; i < samplesLength - delaySamples; i++) {
			allPassTaps[i + delaySamples] += allPassTaps[i] * double(decay);
		}

		return allPassTaps;
	}



	int WriteAudioFile(const std::vector<std::vector<double>>& audioChannels, const std::string& filePath, double samplingFrequency)
	{
		int error = 0;
		size_t channels = audioChannels.size();
		size_t nSamples = channels * audioChannels[0].size();
		double* rawData = new double[nSamples];
		
		double* ptr = rawData;
		for (size_t i = 0; i < audioChannels[0].size(); i++) {
			for (int j = 0; j < audioChannels.size(); j++) {
				double* channelPtr = ptr + j;
				*channelPtr = audioChannels[j][i];
			}
			ptr += channels;
		}

		SF_INFO wavInfo = SF_INFO();
		wavInfo.samplerate = (int)samplingFrequency;
		wavInfo.channels =  (int)channels;
		wavInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
		
		
		SNDFILE* sndFile = sf_open((const char*)filePath.c_str(), SFM_WRITE, &wavInfo);
		
		if (sf_error(sndFile) != SF_ERR_NO_ERROR) { unda::utils::logError(sf_strerror(sndFile), unda::errorSeverity::CRITICAL); error = -1; }
		
		sf_count_t written = sf_writef_double(sndFile, (const double*)rawData, (sf_count_t)nSamples);
		
		sf_close(sndFile);
		if (sf_error(sndFile) != SF_ERR_NO_ERROR) { unda::utils::logError(sf_strerror(sndFile), unda::errorSeverity::CRITICAL); error = -1; }

		delete[] rawData;
		return error;
	}


	// Handles LPF and HPF case
	Filter::Filter(filterType filt_t, int num_taps, double Fs, double Fx)
	{
		errorFlag = 0;
		type = filt_t;
		nTaps = num_taps;

		if (Fs <= 0) setError(-1);
		if (Fx <= 0 || Fx >= Fs / 2) setError(-2);
		fs = Fs;
		omega = Fx / (fs / 2.0);

		taps.resize(nTaps);
		convolver.reset(new fftconvolver::FFTConvolver());

		switch (type) {
		case filterType::LPF:
			designLPF(); break;
		case filterType::HPF:
			designHPF(); break;
		case filterType::ALLPASS:
			designAllPass(); break;
		default:
			setError(-5); break;
		}
		//if (getError() == 0)
			//convolver->init(unda::dspBlockSize, &taps[0], taps.size());
	}

	// Handles BPF case
	Filter::Filter(filterType filt_t, int num_taps, double Fs, double Fl,
		double Fu)
	{
		errorFlag = 0;
		type = filt_t;
		nTaps = num_taps;
		if (Fs <= 0) setError(-1);
		if (Fl <= 0 || Fl >= Fs / 2) setError(-12);
		if (Fu <= 0 || Fu >= Fs / 2) setError(-13);
		fs = Fs;
		omegaLower = Fl / (fs / 2.0);
		omegaUpper = Fu / (fs / 2.0);

		taps.resize(nTaps);
		convolver.reset(new fftconvolver::FFTConvolver());
		if (type == filterType::BPF) designBPF();
		else setError(-16);
		//if (getError() == 0)
			//convolver->init(unda::dspBlockSize, &taps[0], taps.size());
	}

	Filter::~Filter()
	{
		taps.clear();
	}

	void Filter::convolveToSignal(Signal& signal)
	{


		std::vector<float> ir, output;
		ir.resize(taps.size());
		output.resize(signal.size());
		getTaps((float*)&ir[0]);
		//fftconvolver::FFTConvolver* convolver = new fftconvolver::FFTConvolver();
		//bool convolverOk = convolver->init(256, (float*)&ir[0], ir.size());
		//if (!convolverOk) {
		//	UNDA_ERROR("Cannot initialise convolver!")
		//}

		//convolver->process((float*)&signal[0], (float*)&output[0], signal.size());
1		signal = Convolve(signal, taps);
		//signal.clear();
		//signal = Signal(output.begin(), output.end());
		//delete convolver;

	}
	Signal Filter::convolveToSignal(const Signal& signal) {
		std::vector<float> ir, output;
		ir.resize(taps.size());
		output.resize(signal.size());
		getTaps((float*)&ir[0]);

		fftconvolver::FFTConvolver* convolver = new fftconvolver::FFTConvolver();
		convolver->init(signal.size(), (float*)&ir[0], ir.size());

		convolver->process((float*)&signal[0], (float*)&output[0], signal.size());
		delete convolver;
		return Signal(output.begin(), output.end());
	}


	void Filter::designLPF()
	{
		double arg;
		for (int i = 0; i < nTaps; i++) {
			arg = (double)i - ((double)nTaps - 1.0) / 2.0;
			taps[i] = omega * maths::sinc(omega * arg * maths::pi);
		}
	}

	void Filter::designHPF()
	{
		double arg;
		if (nTaps % 2 == 1) // Odd taps
		{
			for (int i = 0; i < nTaps; i++) {
				arg = (double)i - ((double)nTaps - 1.0) / 2.0;
				taps[i] = maths::sinc(arg * maths::pi) - omega * maths::sinc(omega * arg * maths::pi);
			}
		}
		else // Even number of taps
		{
			for (int i = 0; i < nTaps; i++) {
				arg = (double)i - ((double)nTaps - 1.0) / 2.0;
				if (arg == 0.0) taps[i] = 0.0;
				else taps[i] = cos(omega * arg * maths::pi) / maths::pi / arg + cos(arg * maths::pi);
			}
		}
	}

	void Filter::designAllPass()
	{
		float decay = 0.131f;
	}

	void Filter::designBPF()
	{
		double arg;
		for (int i = 0; i < nTaps; i++) {
			arg = (double)i - ((double)nTaps - 1.0) / 2.0;
			if (arg == 0.0) taps[i] = 0.0;
			else taps[i] = (cos(omegaLower * arg * maths::pi) - cos(omegaUpper * arg * maths::pi)) / maths::pi / arg;
		}

	}
}