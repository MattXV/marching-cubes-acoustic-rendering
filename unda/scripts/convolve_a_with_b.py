from pathlib import Path

import numpy as np
from scipy import signal
import soundfile as sf


A = 'drums.wav'
IRs = ['Room_GT.wav', 'ir.wav']
OUT_SAMPLERATE = 48000


normalise = lambda x: x / np.abs(np.max(x))


def to_mono(signal_left, signal_right):
	return signal_left[:] + signal_right[:]


x, x_samplerate = sf.read(A)
x = x[:, 0] + x[:, 1]
t = np.max(x.shape) / x_samplerate
x = signal.resample(x, int(np.floor(t * OUT_SAMPLERATE)))
x = normalise(x)
print('x samplerate: ', x_samplerate)
print('resampled x: ', x.shape)


for ir_name in IRs:
	path = Path(ir_name)
	name = path.stem

	ir, ir_samplerate = sf.read(str(path.resolve()))
	if len(ir.shape) > 1:
		ir = to_mono(ir[:, 0], ir[:, 1])
	t = np.max(ir.shape) / ir_samplerate
	ir = signal.resample(ir, int(np.floor(t * OUT_SAMPLERATE)))
	ir = normalise(ir)
	print('ir samplerate: ', ir_samplerate)
	print('resampled ir: ', ir.shape)

	y = np.convolve(ir, x, 'full')
	y = normalise(y)

	sf.write('out_{}.wav'.format(name), y, OUT_SAMPLERATE)
