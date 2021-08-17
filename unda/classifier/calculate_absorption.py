import json
import numpy as np
import pandas as pd

walls = ['leftWall', 'rightWall', 'ceiling', 'floor', 'backWall', 'frontWall']

with open('../conf.json', 'r') as file:
    conf = json.load(file)
surface_subdivision = int(conf['GeometryReduction']['MarchingCubesResolution'])
total_tiles = surface_subdivision**2 * 6


results = pd.read_csv('results.csv')
patches = results.loc[(results['Confidence'] > 0.95)]


tiles_per_wall = surface_subdivision**2
out = np.zeros((6, 6), np.float32)
tiles_wall = surface_subdivision**2

for i, wall in enumerate(walls):
    wall_patches = patches.loc[patches['Face'] == wall]
    wall_patches = wall_patches.iloc[:, -6:].to_numpy(np.float32)
    # if wall_patches.shape[0] > tiles_per_wall:
    #     wallPatches = wall_patches[:tiles_per_wall]

    # wall_tiles = np.ones((tiles_per_wall, 6), np.float32)

    # weights = np.zeros(tiles_per_wall, np.float32)
    # weights[:wallPatches.shape[0]] = wallPatches.shape[0] / tiles_per_wall
    # weights[wallPatches.shape[0]:] = (tiles_per_wall - wallPatches.shape[0]) / tiles_per_wall
    # wall_tiles[:wallPatches.shape[0]] = wallPatches
    # out[i] = np.average(wall_tiles, 0, weights)
    out[i] = np.broadcast_to(np.mean(wall_patches, 0), out[i].shape)


out = np.clip(out, 0.001, 0.9)
conf['IR']['SurfaceAbsorption'] = out.tolist()

with open('../conf.json', 'w') as file:
    json.dump(conf, file, indent=4)