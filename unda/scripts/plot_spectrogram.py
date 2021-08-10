import audio_tools
import matplotlib.pyplot as plt
import soundfile as sf
import matplotlib

font = {'family' : 'normal',
        'weight' : 'bold',
        'size'   : 18}

matplotlib.rc('font', **font)
IR_PATH = '../ir.wav'

ir, fs = sf.read(IR_PATH)

fig, ax = plt.subplots()

audio_tools.plot_spectrogram(ir, fs, ax, 'IR', scale=900)

plt.show()