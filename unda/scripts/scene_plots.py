import os
from signal import read_audio, SAMPLERATE, plot_waveform, plot_spectrogram, \
    trim_from_to, plot_image, get_rir_segments, get_c50, get_d50, plot_db_scale, get_rt60
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
room = read_audio('Room.wav', SAMPLERATE)
room_gt = read_audio('Room_GT.wav', SAMPLERATE)


office_scene = read_image(os.path.join(images, 'Office.PNG'), image_size)
office_mc = read_image(os.path.join(images, 'Office_mc.PNG'), image_size)
office = read_audio('Office.wav', SAMPLERATE)
office_gt = read_audio('Office_GT.wav', SAMPLERATE)

church_scene = read_image(os.path.join(images, 'Church.PNG'), image_size)
church_mc = read_image(os.path.join(images, 'Church_MarchingCubes.PNG'), image_size)
church = read_audio('Church.wav', SAMPLERATE)
church_gt = read_audio('Church_GT.wav', SAMPLERATE)

village_scene = read_image(os.path.join(images, 'Village.PNG'), image_size)
village_mc = read_image(os.path.join(images, 'Village_mc.PNG'), image_size)
village = read_audio('Village.wav', SAMPLERATE)
village_gt = read_audio('Village_GT.wav', SAMPLERATE)



start = 0.0
end = 0.4


fig, axes = plt.subplots(4, 6, figsize=(18, 12))
fig2, db_axes = plt.subplots(4, 2)
fig3, ax = plt.subplots()

plot_waveform(trim_from_to(room,       start, end), SAMPLERATE, axes[0][2], 'Generated')
plot_waveform(trim_from_to(room_gt,    start, end), SAMPLERATE, axes[0][3], 'Ground Truth')
plot_waveform(trim_from_to(office,     start, end), SAMPLERATE, axes[1][2])
plot_waveform(trim_from_to(office_gt,  start, end), SAMPLERATE, axes[1][3], td=int(0.01 * SAMPLERATE))
plot_waveform(trim_from_to(church,     start, end), SAMPLERATE, axes[2][2])
plot_waveform(trim_from_to(church_gt,  start, end), SAMPLERATE, axes[2][3])
plot_waveform(trim_from_to(village,    start, end), SAMPLERATE, axes[3][2])
plot_waveform(trim_from_to(village_gt, start, end), SAMPLERATE, axes[3][3], td=int(0.01 * SAMPLERATE))


plot_image(room_scene, axes[0][0], 'Room', title='Render')
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

print('{} D50: {}'.format('Room',       get_d50(room, SAMPLERATE)))
print('{} D50: {}'.format('Room GT',    get_d50(room_gt, SAMPLERATE)))
print('{} D50: {}'.format('Church',     get_d50(church, SAMPLERATE)))
print('{} D50: {}'.format('Church GT',  get_d50(church_gt, SAMPLERATE)))
print('{} D50: {}'.format('Office',     get_d50(office, SAMPLERATE)))
print('{} D50: {}'.format('Office GT',  get_d50(office_gt, SAMPLERATE)))
print('{} D50: {}'.format('Village',    get_d50(village, SAMPLERATE)))
print('{} D50: {}'.format('Village GT', get_d50(village_gt, SAMPLERATE)))

print('{} t60: {}'.format('Room',       get_rt60(room, SAMPLERATE)))
print('{} t60: {}'.format('Room GT',    get_rt60(room_gt, SAMPLERATE)))
print('{} t60: {}'.format('Church',     get_rt60(church, SAMPLERATE)))
print('{} t60: {}'.format('Church GT',  get_rt60(church_gt, SAMPLERATE)))
print('{} t60: {}'.format('Office',     get_rt60(office, SAMPLERATE)))
print('{} t60: {}'.format('Office GT',  get_rt60(office_gt, SAMPLERATE)))
print('{} t60: {}'.format('Village',    get_rt60(village, SAMPLERATE)))
print('{} t60: {}'.format('Village GT', get_rt60(village_gt, SAMPLERATE)))

start = 0.0
end = 0.9

office_gt = np.concatenate((np.zeros((32)), np.squeeze(office_gt)))
plot_spectrogram(trim_from_to(room, 0, 3),      SAMPLERATE, axes[0][4],  'Generated', 3800)
plot_spectrogram(trim_from_to(room_gt, 0, 3),   SAMPLERATE, axes[0][5], 'Ground Truth')
plot_spectrogram(trim_from_to(office, 0, 3),    SAMPLERATE, axes[1][4], scale=3100)
plot_spectrogram(trim_from_to(office_gt, 0, 3), SAMPLERATE, axes[1][5])
plot_spectrogram(trim_from_to(church, 0, 10),    SAMPLERATE, axes[2][4], scale=3200)
plot_spectrogram(trim_from_to(church_gt, 0, 10), SAMPLERATE, axes[2][5])
plot_spectrogram(trim_from_to(village, 0, 3),   SAMPLERATE, axes[3][4], scale=2100)
plot_spectrogram(trim_from_to(village_gt, 0, 3),   SAMPLERATE, axes[3][5])

_, td, _, _, _ = get_rir_segments(room, SAMPLERATE)
ax.plot(trim_from_to(room[td:], 0, 0.2), 'b')

# plot_db_scale(trim_from_to(room, 0, 3),       SAMPLERATE, db_axes[0][0], 'Room')
# plot_db_scale(trim_from_to(room_gt, 0, 3),    SAMPLERATE, db_axes[0][1], 'RoomGT')
# plot_db_scale(trim_from_to(office, 0, 3),     SAMPLERATE, db_axes[1][0], 'Office')
# plot_db_scale(trim_from_to(office_gt, 0, 3),  SAMPLERATE, db_axes[1][1], 'OfficeGT')
# plot_db_scale(church,     SAMPLERATE, db_axes[2][0], 'Church')
# plot_db_scale(church_gt,  SAMPLERATE, db_axes[2][1], 'ChurchGT')
# plot_db_scale(trim_from_to(village, 0, 3),    SAMPLERATE, db_axes[3][0], 'Village')
# plot_db_scale(trim_from_to(village_gt, 0, 3), SAMPLERATE, db_axes[3][1], 'VillageGT')



plt.tight_layout(pad=1.8, h_pad=0.028, w_pad=-1.07)
plt.show()
