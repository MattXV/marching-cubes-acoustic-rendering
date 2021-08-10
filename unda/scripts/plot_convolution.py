import numpy as np
import audio_tools as at
from pathlib import Path
import matplotlib.pyplot as plt

plt.style.use(['science'])

SAMPLERATE = 44100


drums = at.read_audio('../drums.wav', SAMPLERATE)
rir = at.read_audio('Room_GT.wav', SAMPLERATE)
convolved = at.convolve_ir_to_signal(drums, SAMPLERATE, rir, SAMPLERATE, SAMPLERATE)

fig, (ax, bx) = plt.subplots(2)

t = np.max(drums.shape) / SAMPLERATE
convolved = at.trim_from_to(convolved, 0, t + 1, fs=SAMPLERATE)

at.plot_spectrogram(drums, SAMPLERATE, ax, label='Original', scale=500)
at.plot_spectrogram(convolved, SAMPLERATE, bx, label='Convolved', scale=500)


# plt.tight_layout()
plt.show()
