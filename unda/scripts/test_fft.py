import numpy as np
from filters import Filter
from numpy.fft import rfft, ifft
from scipy.signal import spectrogram
import matplotlib.pyplot as plt
import soundfile as sf


N_TAPS = 2**16 + 1
LOWER_F = 400
UPPER_F = 1000
FS = 44100

# fig, ax = plt.subplots()

# x, fs = sf.read('test_kernel.wav')
# x_fft = rfft(x)
# x_fft = np.abs(x_fft[:x_fft.shape[0] // 2].real)
# f = np.linspace(0, fs / 2, x_fft.shape[0])
# ax.loglog(f, x_fft)
# # ax.plot(x)
# plt.show()
# quit()


def db_to_mag(db):
    return 10 ** (np.copy(db) / 20)

def mag_to_db(signal, threshold=-80):
    input_signal = np.abs(np.squeeze(np.copy(signal)))
    thresh = db_to_mag(threshold)
    x = np.ones(max(input_signal.shape), np.float32) * thresh
    x = np.where(input_signal > thresh, input_signal, x)
    return 20 * np.log10(x)


def convolve(signal, kernel, window=None):
    if window:
        kernel = kernel * window(kernel.shape[0])
    n_conv = signal.shape[0] + kernel.shape[0] - 1
    signal_fft = rfft(signal, n_conv)
    kernel_fft = rfft(kernel, n_conv)
    convolution = signal_fft * kernel_fft
    out = ifft(convolution, n_conv)
    out = out[kernel.shape[0] // 2:]
    out = out[:signal.shape[0]]
    # out[:100] *= np.linspace(0, 1, 100)
    # out[-100:] *= np.linspace(1, 0, 100)
    return np.array(out.real, np.float32)

# PLOTS
def plot_spectrogram(signal, samplerate, axes, label=None, scale=1000):
    # Pxx, freqs, bins, im = axes.specgram(signal[:, 0],
    #     Fs=samplerate, sides='onesided', mode='psd',
    #     pad_to=SPECTROGRAM_NSEGS, NFFT=SPECTROGRAM_NFFT,
    #     scale='dB', cmap='coolwarm', scale_by_freq=False)
    y = np.copy(signal)
    y = y * scale
    y = np.squeeze(y)
    y = np.where(y >= 1, 0.9, y)
    y = np.where(y <= -1, -0.9, y)
    f, t, Sxx = spectrogram(np.squeeze(y), samplerate, window=np.blackman(256),
                            nfft=2**14, mode='magnitude')
    # norm = Normalize(clip=True)
    axes.pcolormesh(t, f, Sxx, cmap='gist_heat')
    if label:
        axes.set_title(label, fontsize=18)
    axes.set_ylabel('Frequency (Hz)', fontsize=12)
    axes.set_xlabel('Time (s)', fontsize=12)
    axes.set_yscale('log')
    axes.set_ylim(20, 25000)


irs = [
    'frequency_bin_0.wav',
    'frequency_bin_1.wav',
    'frequency_bin_2.wav',
    'frequency_bin_3.wav',
    'frequency_bin_4.wav',
    'frequency_bin_5.wav'
]

hpf = Filter('hpf', 20, 20, n_coeffs=2**15)
lpf = Filter('bpf', 2000, 20000)

fig, (ax, bx) = plt.subplots(2)
hpf_fft = rfft(lpf.coeffs)

hpf_fft = np.abs(hpf_fft[:hpf_fft.shape[0] // 2].real)

f = np.linspace(0, 1, hpf_fft.shape[0])
ax.plot(lpf.coeffs)
bx.loglog(hpf_fft)

# bpf = Filter('hpf', 10000, 10000, n_coeffs=2*17)
noise = np.random.uniform(-1, 1, 44100 * 1)
sf.write('noise.wav', noise, 44100)
noise = convolve(noise, lpf.coeffs)
noise = noise / np.max(np.abs(noise))
sf.write('noise_.wav', noise, 44100)

plt.show()
quit()

bpf_0 = Filter('bpf', 20, 125, n_coeffs=2**16 + 1)
bpf_1 = Filter('bpf', 125, 250, n_coeffs=2**16 + 1)
bpf_2 = Filter('bpf', 250, 500, n_coeffs=2**16 + 1)
bpf_3 = Filter('bpf', 500, 1000, n_coeffs=2**16 + 1)
bpf_4 = Filter('bpf', 1000, 2000, n_coeffs=2**16 + 1)
bpf_5 = Filter('bpf', 2000, 20000, n_coeffs=2**16 + 1)
firs = [bpf_0, bpf_1, bpf_2, bpf_3, bpf_4, bpf_5]


out = None

fig, (ax, bx, cx) = plt.subplots(3)
labels = ['20-125', '125-250', '250-500', '500-1000', '1000-2000', '2000-20000']
for fir, label in zip(firs, labels):
    fir_fft = rfft(fir.coeffs)
    fir_fft = np.abs(fir_fft[:fir_fft.shape[0] // 2])
    f = np.linspace(0, fir.fs / 2, fir_fft.shape[0])
    ax.loglog(f, fir_fft, label=label)


for ir, fir, label in zip(irs, firs, labels):
    ir, fs = sf.read(ir)
    if out is None:
        out = np.zeros_like(ir, np.float32)
    ir = convolve(ir, fir.coeffs, np.hanning)
    
    ir_fft = rfft(ir)
    ir_fft = np.abs(ir_fft[:ir_fft.shape[0] // 2])
    f = np.linspace(0, fs / 2, ir_fft.shape[0])
    bx.loglog(f, ir_fft, label=label)
    out += ir


out = out / np.max(np.abs(out))
cx.plot(out)
# plot_spectrogram(out, 44100, cx, 'IR', 2000)
ax.legend()
bx.legend()
plt.show()