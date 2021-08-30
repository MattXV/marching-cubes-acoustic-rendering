import json
import numpy as np
import matplotlib.pyplot as plt
import pyroomacoustics as pra
from scipy.io import wavfile
import soundfile as sf

def xyz_to_xzy(vec3):
    vec = vec3.copy()
    return (vec[0], vec[2], vec[1])


with open('conf.json', 'r') as file:
    conf = json.load(file)

# The desired reverberation time and dimensions of the room
room_dim = xyz_to_xzy(conf['Scene']['Dimensions'])  # meters
# import a mono wavfile as the source signal
# the sampling frequency should match that of the room
# fs = 44100
signal, fs = sf.read('drums.wav')
# We invert Sabine's formula to obtain the parameters for the ISM simulator
# e_absorption, max_order = pra.inverse_sabine(rt60_tgt, room_dim)

room_absorption = conf['IR']['SurfaceAbsorption']
materials = dict()
material_keys = ['west', 'east', 'ceiling', 'floor', 'north', 'south']
for i, key in enumerate(material_keys):
    energy_absorption = {'description': 'custom',
                     'coeffs': room_absorption[i],
                     'center_freqs': [125, 250, 500, 1000, 2000, 4000]
    }
    materials[key] = pra.Material(energy_absorption=energy_absorption)


max_order = 9

# Create the room
room = pra.ShoeBox(
    room_dim, fs=fs, materials=materials, max_order=max_order
)

# place the source in the room
room.add_source(xyz_to_xzy(conf['IR']['SourcePosition']), signal=signal, delay=0)

# define the locations of the microphones
mic_locs = np.c_[
    xyz_to_xzy(conf['IR']['ListenerPosition']), xyz_to_xzy(conf['IR']['ListenerPosition']) # mic 1  # mic 2
]


# finally place the array in the room
room.add_microphone_array(mic_locs)

# Run the simulation (this will also build the RIR automatically)
room.image_source_model()
room.compute_rir()
rt60 = room.measure_rt60(plot=False)

for m in range(room.n_mics):
    for s in range(room.n_sources):
        print(
            "RT60 between the {}th mic and {}th source: {:.3f} s".format(m, s, rt60[m, s])
        )
# measure the reverberation time
# plt.plot(room.rir[1][0])


rir = room.rir[0][0] + room.rir[1][0]
# rir = rir / np.max(np.abs(rir))

sf.write('pyroomacoustics_rir.wav', room.rir[0][0], fs)
# plt.show()

# plot one of the RIR. both can also be plotted using room.plot_rir()
# fig = plt.figure()
# room.plot(image_order=3, aspect='equal')
# room.plot_rir()
# plt.show()
