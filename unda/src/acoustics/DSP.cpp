#include "DSP.h"


namespace unda {
	using maths::pi;

	void unda::NormaliseSignal(Signal& audioSamples)
	{
		Sample value = 0.0, tempValue;
		for (Sample& sample : audioSamples) {
			tempValue = abs(sample);
			if (tempValue > value) value = tempValue;
		}
		for (Sample& sample : audioSamples) {
			sample = (sample / value);
		}
	}

	Signal unda::NormaliseSignal(const Signal& audioSamples)
	{
		Signal output;
		Sample value = 0.0, tempValue;
		for (const Sample& sample : audioSamples) {
			tempValue = abs(sample);
			if (tempValue > value) value = tempValue;
		}
		for (Sample sample : audioSamples)
			output.push_back(sample / value);
		return output;
	}

	void ZeroCrossingFadeInOut(Signal& signal) {
		for (size_t i = 0; i < signal.size() - 1; i++) {
			if (unda::maths::SameSign(signal[i], signal[i + 1]))
				signal[i] = 0;
			else break;
		}
		for (int i = (int)signal.size() - 1; i > 0; i--) {
			if (unda::maths::SameSign(signal[i], signal[(size_t)i - 1]))
				signal[i] = 0;
			else break;
		}
	}

	int WriteAudioFile(const std::vector<Signal>& audioChannels, const std::string& filePath, double samplingFrequency)
	{
		int error = 0;
		size_t channels = audioChannels.size();
		size_t nSamples = channels * (size_t)audioChannels[0].size();
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
		if (sf_error(sndFile) != SF_ERR_NO_ERROR) { UNDA_ERROR("Could not open " + filePath + " ..."); error = -1; }
		sf_count_t written = sf_writef_double(sndFile, (const double*)rawData, (sf_count_t)nSamples);
		sf_close(sndFile);
		delete[] rawData;
		return error;
	}

	Signal ReadAudioFileIntoMono(const std::string& filePath)
	{
		Signal out;
		SF_INFO info;
		SNDFILE* sndFile = sf_open(filePath.c_str(), SFM_READ, &info);
		if ((int)info.samplerate != (int)unda::sampleRate) { UNDA_ERROR("Eror in file: " + filePath + " SampleRate has to be " + std::to_string((int)unda::sampleRate)); }
		else if (info.frames == 0) { UNDA_ERROR("No frames in: " + filePath); }
		else {
			float* data = new float[(size_t)info.frames * (size_t)info.channels];
			size_t nRead = (size_t)sf_read_float(sndFile, data, info.frames * info.channels);
			out.resize(nRead);
			float* iterator = data;
			for (size_t i = 0; i < nRead; i++) {
				float sample = 0;
				for (size_t channel = 0; channel < (size_t)info.channels; channel++) {
					sample += float(*(iterator + channel));
				}
				out[i] = sample;
				iterator += info.channels;
			}
			NormaliseSignal(out);
			delete[] data;
		}
		sf_close(sndFile);
		return out;
	}


	Signal FFTConvolution(const Signal& signal, const Signal& kernel)
	{
		size_t nConv = signal.size() + kernel.size() - 1; // Convolved Length = len_input + len_kernel - 1
		size_t N = (size_t)unda::roundUpToNextPowerOfTwo((unsigned int)nConv); // PFFFT wants powers of two for efficiency
		PFFFT_Setup* fftSetup = pffft_new_setup((int)N, pffft_transform_t::PFFFT_REAL);
		float* paddedSignal = new float[N];
		float* paddedKernel = new float[N];
		float* workSpace    = new float[N];
		float* convolution  = new float[N];
		for (size_t i = 0; i < N; i++) {
			// Zero padding inputs
			paddedKernel[i] = i < kernel.size() ? kernel[i] : 0.0f;
			paddedSignal[i] = i < signal.size() ? signal[i] : 0.0f;
			convolution[i] = 0;
		}	
		pffft_transform(fftSetup, paddedSignal, paddedSignal, workSpace, pffft_direction_t::PFFFT_FORWARD);
		pffft_transform(fftSetup, paddedKernel, paddedKernel, workSpace, pffft_direction_t::PFFFT_FORWARD);
		// Always use zconvolve_accumulate to multiply spectrums together as PFFFT uses some satanic frequency ordering.
		// Unless you use transform_ordered(). Good luck figuring that out.
		// pffft stands for "pffft, you don't even know how to fft, bro".
		pffft_zconvolve_accumulate(fftSetup, paddedSignal, paddedKernel, convolution, 1.0);
		pffft_transform(fftSetup, convolution, convolution, workSpace, pffft_direction_t::PFFFT_BACKWARD);
		Signal out = Signal(nConv, 0);
		for (size_t i = 0; i < nConv; i++) {
						         // Removing initial time shift
			out[i] = convolution[i + kernel.size() / 2] / (float)N; // Rescaling as PFFFT_BACKWARD(PFFFT_FORWARD(x)) = N*x
		}
		delete[] paddedSignal;
		delete[] paddedKernel;
		delete[] workSpace;
		delete[] convolution;
		pffft_destroy_setup(fftSetup);
		return out;
	}


	Signal designLPF(unsigned int M, float fc)
	{
		// Windowed-sinc FIR low-pass using Blackman
		// ---  Smith S. W. The Scientist and Engineer's guide to DSP --- 
		Signal h = Signal((size_t)M, Sample());
		float n, sum = 0;
		for (unsigned int i = 0; i < M; i++) {
			n = (Sample)i - (Sample)M / 2;
			if (n == 0.0)
				h[i] = 2 * (Sample)pi * fc;
			else
				h[i] = sinf(2.0f * (Sample)pi * fc * n) / n;
			// the filter is already built at this stage. The next line applies a Blackman window to it.
			h[i] = h[i] * (0.42f - 0.5f * cosf(2.0f * (Sample)pi * (Sample)i / (Sample)M) + 0.08f * cosf(4.0f * (Sample)pi * (Sample)i / (Sample)M));
			sum += h[i];
		}
		// DC normalisation
		for (unsigned int i = 0; i < M; i++)
			h[i] = h[i] / sum;
		return h;
	}


	Signal designHPF(unsigned int M, float fc)
	{
		Signal h = designLPF(M, fc);
		// high-pass FIR using spectral inversion.
		for (unsigned int i = 0; i < M; i++)
			h[i] = -h[i];
		h[(size_t)(M / 2)] += 1;
		return h;
	}


	Signal designBPF(unsigned int M, float lower_fc, float upper_fc)
	{
		Signal h = designLPF(M, upper_fc), hpf = designHPF(M, lower_fc);
		// Band-pass FIR combining the above.
		for (unsigned int i = 0; i < M; i++) {
			h[i] = h[i] + hpf[i];
			h[i] = -h[i];
		}
		h[(size_t)(M / 2)] += 1;
		return h;
	}


	Filter::Filter(filterType filt_t, float cutoff_hz)
	{
		lower_fc = cutoff_hz / fs;
		UNDA_ASSERT(lower_fc > 0 && lower_fc < 0.5);
		UNDA_ASSERT(fs > 0);
		UNDA_ASSERT(filt_t != filterType::BPF);
		if (filt_t == filterType::LPF)	h = designLPF(M, lower_fc);
		else							h = designHPF(M, lower_fc);
	}


	Filter::Filter(filterType filt_t, float lower_cutoff_hz, float upper_cutoff_hz)
	{
		lower_fc = lower_cutoff_hz / fs;
		upper_fc = upper_cutoff_hz / fs;
		UNDA_ASSERT(lower_fc > 0 && lower_fc < 0.5);
		UNDA_ASSERT(upper_fc > 0 && upper_fc < 0.5);
		UNDA_ASSERT(fs > 0);
		UNDA_ASSERT(filt_t == filterType::BPF);
		h = designBPF(M, lower_fc, upper_fc);
	}
}