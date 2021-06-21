import os
from colorsys import hsv_to_rgb
from pathlib import Path
from glob import glob
import numpy as np
from scipy import interpolate, integrate
from scipy.io import wavfile
import soundfile as sf
from scipy.signal import correlate, deconvolve, find_peaks, spectrogram, windows, resample
import matplotlib.pyplot as plt

SAMPLERATE = 44100

def normalise(signal):
    return signal / np.abs(np.max(signal))


def interp_signal(signal, points):
    interp = interpolate.interp1d(np.arange(signal.shape[0]), signal)
    x = np.linspace(0, signal.shape[0] - 1, points)
    return x, interp(x)


def mag_to_db(signal):
    x = np.ones(max(signal.shape), np.float32) * 1e-7
    x = np.where(signal > 0, signal, x)
    return 20 * np.log10(x)


def db_to_mag(db):
    return 10 ** (db / 20)


def get_rir_segments(rir, fs):
    x = mag_to_db(rir ** 2)

    # direct sound
    peaks, _ = find_peaks(x, prominence=db_to_mag(-30), distance=int(fs * 0.1 * 10e-3))
    max_peak = np.argsort(x[peaks])[::-1]
    td = peaks[max_peak[0]]

    # Segment power function. De Lima et al.
    direct = x[:int(td - 1 * 10e-3 * fs)]
    ed = x[int(td - (1 * 10e-3) * fs):int(td + (1.5 * 10e-3) * fs)]
    er = x[int(td + (1.5 * 10e-3) * fs):]

    return x, ed, er, peaks


def read_audio(file, out_samplerate):
    x, fs = sf.read(file, always_2d=True)
    if (x.shape[1] == 2):
        x = x[:, 0] + x[:, 1]
    x = normalise(x)
    t = np.max(x.shape) / fs
    x = resample(x, int(np.floor(t * fs)))
    x = normalise(x)
    return x


fig_spec, ax_spec = plt.subplots(nrows=3)
fig_wave, ax_wave = plt.subplots(nrows=3)
village = read_audio('Village2.wav', SAMPLERATE)
room = read_audio('ConferenceRoom.wav', SAMPLERATE)


# Plotting Time domain
t = np.max(village.shape) / SAMPLERATE
fig_wave.suptitle('Impulse Responses (h)')
ax_wave[0].plot(np.linspace(0, t, village.shape[0]), village)
ax_wave[0].set_xlabel('Time (s)')
ax_wave[0].set_ylabel('Magnitude')
f, ts, Sxx = spectrogram(village, SAMPLERATE, window='triang')
print(f)
ax_spec[0].pcolormesh(ts, f, Sxx, cmap='gist_heat')


plt.tight_layout(pad=1.2)
plt.show()
