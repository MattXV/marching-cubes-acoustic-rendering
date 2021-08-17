from os import path
from pathlib import Path
import csv
from glob import glob
import json
import numpy as np
import tensorflow as tf
import pandas as pd
import cold_wax
import cv2


physical_devices = tf.config.list_physical_devices('GPU')
tf.config.experimental.set_memory_growth(physical_devices[0], True) 

FACES = ['ceiling', 'floor', 'leftWall', 'rightWall', 'backWall', 'frontWall']

def preprocess(x):
    return x * 2 - 1


PATCHES_DIR = '../output/patches/'
WEIGHTS_PATH = 'cache/PatchClassifier.h5'
CLASSES_PATH = 'cache/PatchClassifierClasses.json'
ONE_TO_MANY = 'onetomany_mapping.json'
ABSORPTION_DATA = 'absorption.xlsx'
INPUT_SHAPE = (32, 32, 3)

classes = json.load(open(CLASSES_PATH, 'r'))['Classes']
classifier = cold_wax.models.PatchClassifier(len(classes), 1, INPUT_SHAPE)
classifier(tf.zeros((64,) + INPUT_SHAPE))
classifier.load_weights(WEIGHTS_PATH)

classifier.summary()

mapping = json.load(open(ONE_TO_MANY))
acoustic_materials = pd.read_excel(ABSORPTION_DATA)


predictions = list()
for image_file in Path(PATCHES_DIR).glob('*.png'):
    path = Path(image_file)
    face = str(Path(str(path).split('_')[-1]).stem)
    print('Working on {:>80}'.format(str(path)), end='\r')
    image = cold_wax.image.read_image(str(path.resolve()), None, False)
    
    if image.shape[0] * image.shape[1] < INPUT_SHAPE[0] * INPUT_SHAPE[1]:
        continue

    image = cv2.resize(image, INPUT_SHAPE[:2])

    x = np.array(image[..., :3], dtype=np.float32) / 255
    x = np.expand_dims(x, 0)
    x = preprocess(x)
    y = np.squeeze(classifier(x))
    label = np.argmax(y)
    label_name = classes[label]
    acoustic_label = mapping[label_name]
    mean_coeffs = acoustic_materials.loc[(acoustic_materials.Category == acoustic_label)].iloc[:, -6:]
    mean_coeffs = mean_coeffs.to_numpy(np.float32).mean(axis=0)

    patch_info = path.stem.split('_')
    assert patch_info[0] in FACES
    
    # row: patch name | Face | class  |  confidence | absorprtion [float x 6]
    row = [patch_info[1], patch_info[0], label_name, np.max(y)]
    row.extend(list(mean_coeffs))
    predictions.append(row)

try:
    with open('results.csv', 'w', newline='') as f:
        writer = csv.writer(f)
        header = ['PatchName', 'Face', 'Label', 'Confidence']
        header.extend(['125', '250', '500', '1000', '2000', '4000'])
        writer.writerow(header)

        writer.writerows(predictions)
except OSError as e:
    print('Could not open results file! Writing to a temp file! Error: \
           OS Error {}'.format(e.errno))
    with open('results_temp.csv', 'w', newline='') as f:
        writer = csv.writer(f)
        header = ['PatchName', 'Face', 'Label', 'Confidence']
        header.extend(['125', '250', '500', '1000', '2000', '4000'])
        writer.writerow(header)

        writer.writerows(predictions)
print('Done! :D')

