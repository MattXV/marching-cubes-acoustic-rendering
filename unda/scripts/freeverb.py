import soundfile as sf
import numpy as np


def freeverb_main(input_, num_samples):
    out = np.zeros_like(input_)
    for i in range(num_samples):
        


fs = 44100
ir_len = 1.2
x = np.zeros((fs * ir_len), np.float32)



sf.write('ir_test.wav', x, fs);