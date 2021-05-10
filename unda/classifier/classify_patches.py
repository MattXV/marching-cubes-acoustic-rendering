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


def preprocess(x):
    return x * 2 - 1


PATCHES_DIR = '../output/'
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
patches = glob(path.join(PATCHES_DIR, '*.png'))

predictions = list()
for image_file in patches:
    path = Path(image_file)
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
    # row: patch name | class  |  confidence | absorprtion [float x 6]
    row = [path.stem, label_name, np.max(y)]
    row.extend(list(mean_coeffs))
    predictions.append(row)


with open('results.csv', 'w', newline='') as f:
    writer = csv.writer(f)
    header = ['PatchName', 'Label', 'Confidence']
    header.extend(['125', '250', '500', '1000', '2000', '4000'])
    writer.writerow(header)

    writer.writerows(predictions)