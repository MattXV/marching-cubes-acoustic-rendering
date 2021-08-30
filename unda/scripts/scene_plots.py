import os
from audio_tools import read_audio, SAMPLERATE, plot_waveform, plot_spectrogram, \
    trim_from_to, plot_image, get_rir_segments, get_c50, get_d50, plot_db_scale, get_t60
import matplotlib.pyplot as plt
from cold_wax.image import read_image
import numpy as np

# plt.rcParams.update({
    # "font.family": "serif",   # specify font family here
    # "font.serif": ["Times"],  # specify font here
    # "font.size":0.75})          # specify font size here
plt.style.use(['science'])

filenames = ['Room.wav', 'Room_GT.wav',
             'Office.wav', 'Office_GT.wav',
             'Church.wav', 'Church_GT.wav', 
             'Village.wav', 'Village_GT.wav']

images = os.path.join('./', 'scene_images')
image_size = (1000, 1000)

room_scene = read_image(os.path.join(images, 'Room.PNG'), image_size)
room_mc = read_image(os.path.join(images, 'Room_mc.PNG'), image_size)
office_scene = read_image(os.path.join(images, 'Office.PNG'), image_size)
office_mc = read_image(os.path.join(images, 'Office_mc.PNG'), image_size)
church_scene = read_image(os.path.join(images, 'Church.PNG'), image_size)
church_mc = read_image(os.path.join(images, 'Church_MarchingCubes.PNG'), image_size)
village_scene = read_image(os.path.join(images, 'Village.PNG'), image_size)
village_mc = read_image(os.path.join(images, 'Village_mc.PNG'), image_size)

room       = np.abs(read_audio('Room.wav', SAMPLERATE))
room_gt    = np.abs(read_audio('Room_GT.wav', SAMPLERATE))
office     = np.abs(read_audio('Office.wav', SAMPLERATE))
office_gt  = np.abs(read_audio('Office_GT.wav', SAMPLERATE))
church     = np.abs(read_audio('Church.wav', SAMPLERATE))
church_gt  = np.abs(read_audio('Church_GT.wav', SAMPLERATE))
village    = np.abs(read_audio('Village.wav', SAMPLERATE))
village_gt = np.abs(read_audio('Village_GT.wav', SAMPLERATE))



start = 0.0
end = 0.4


fig, axes = plt.subplots(4, 6, figsize=(14, 6))
# fig2, db_axes = plt.subplots(4, 2)
# fig3, ax = plt.subplots()

# plot_waveform(room,       SAMPLERATE, axes[0][2], 'Ours')
# plot_waveform(room_gt,    SAMPLERATE, axes[0][3], 'Ground Truth')
# plot_waveform(office,     SAMPLERATE, axes[1][2])
# plot_waveform(office_gt,  SAMPLERATE, axes[1][3])
# plot_waveform(church,     SAMPLERATE, axes[2][2])
# plot_waveform(church_gt,  SAMPLERATE, axes[2][3])
# plot_waveform(village,    SAMPLERATE, axes[3][2])
# plot_waveform(village_gt, SAMPLERATE, axes[3][3])

plot_db_scale(room,       SAMPLERATE, axes[0][2], 'Ours')
plot_db_scale(room_gt,    SAMPLERATE, axes[0][3], 'Ground Truth')
plot_db_scale(office,     SAMPLERATE, axes[1][2])
plot_db_scale(office_gt,  SAMPLERATE, axes[1][3])
plot_db_scale(church,     SAMPLERATE, axes[2][2])
plot_db_scale(church_gt,  SAMPLERATE, axes[2][3])
plot_db_scale(village,    SAMPLERATE, axes[3][2])
plot_db_scale(village_gt, SAMPLERATE, axes[3][3])

plot_image(room_scene, axes[0][0], 'Room', title='Scene')
plot_image(room_mc, axes[0][1], title='Acoustic Volume')
plot_image(office_scene, axes[1][0], 'Office')
plot_image(office_mc, axes[1][1])
plot_image(church_scene, axes[2][0], 'Church')
plot_image(church_mc, axes[2][1])
plot_image(village_scene, axes[3][0], 'Village')
plot_image(village_mc, axes[3][1])


print('{} c50: {}'.format('Room',       get_c50(room, SAMPLERATE)))
print('{} c50: {}'.format('Room GT',    get_c50(room_gt, SAMPLERATE)))
print('{} c50: {}'.format('Church',     get_c50(church, SAMPLERATE)))
print('{} c50: {}'.format('Church GT',  get_c50(church_gt, SAMPLERATE)))
print('{} c50: {}'.format('Office',     get_c50(office, SAMPLERATE)))
print('{} c50: {}'.format('Office GT',  get_c50(office_gt, SAMPLERATE)))
print('{} c50: {}'.format('Village',    get_c50(village, SAMPLERATE)))
print('{} c50: {}'.format('Village GT', get_c50(village_gt, SAMPLERATE)))
print('------------------------------------------------------------------------')
print('{} D50: {}'.format('Room',       get_d50(room, SAMPLERATE)))
print('{} D50: {}'.format('Room GT',    get_d50(room_gt, SAMPLERATE)))
print('{} D50: {}'.format('Church',     get_d50(church, SAMPLERATE)))
print('{} D50: {}'.format('Church GT',  get_d50(church_gt, SAMPLERATE)))
print('{} D50: {}'.format('Office',     get_d50(office, SAMPLERATE)))
print('{} D50: {}'.format('Office GT',  get_d50(office_gt, SAMPLERATE)))
print('{} D50: {}'.format('Village',    get_d50(village, SAMPLERATE)))
print('{} D50: {}'.format('Village GT', get_d50(village_gt, SAMPLERATE)))
print('------------------------------------------------------------------------')

print('{} t60: {}'.format('Room',       get_t60(room, SAMPLERATE)))
print('{} t60: {}'.format('Room GT',    get_t60(room_gt, SAMPLERATE)))
print('{} t60: {}'.format('Church',     get_t60(church, SAMPLERATE)))
print('{} t60: {}'.format('Church GT',  get_t60(church_gt, SAMPLERATE)))
print('{} t60: {}'.format('Office',     get_t60(office, SAMPLERATE)))
print('{} t60: {}'.format('Office GT',  get_t60(office_gt, SAMPLERATE)))
print('{} t60: {}'.format('Village',    get_t60(village, SAMPLERATE)))
print('{} t60: {}'.format('Village GT', get_t60(village_gt, SAMPLERATE)))

start = 0.0
end = 0.9


plot_spectrogram(room,      SAMPLERATE, axes[0][4],  'Ours')
plot_spectrogram(room_gt,   SAMPLERATE, axes[0][5], 'Ground Truth')
plot_spectrogram(office,    SAMPLERATE, axes[1][4])
plot_spectrogram(office_gt, SAMPLERATE, axes[1][5])
plot_spectrogram(church,    SAMPLERATE, axes[2][4])
plot_spectrogram(church_gt, SAMPLERATE, axes[2][5], scale=4800)
plot_spectrogram(village,   SAMPLERATE, axes[3][4])
plot_spectrogram(village_gt,SAMPLERATE, axes[3][5], scale=3800)

# _, td, _, _, _ = get_rir_segments(room, SAMPLERATE)
# ax.plot(trim_from_to(room[td:], 0, 0.2), 'b')

# plot_db_scale(room,       SAMPLERATE, db_axes[0][0], 'Ours')
# plot_db_scale(room_gt,    SAMPLERATE, db_axes[0][1], 'Ground Truth')
# plot_db_scale(office,     SAMPLERATE, db_axes[1][0])
# plot_db_scale(office_gt,  SAMPLERATE, db_axes[1][1])
# plot_db_scale(church,     SAMPLERATE, db_axes[2][0])
# plot_db_scale(church_gt,  SAMPLERATE, db_axes[2][1])
# plot_db_scale(village,    SAMPLERATE, db_axes[3][0])
# plot_db_scale(village_gt, SAMPLERATE, db_axes[3][1])

# plt.tight_layout(pad=1.8, h_pad=0.028, w_pad=-1.07)
fig.savefig('scene_rirs.png', dpi=400)
plt.show()
