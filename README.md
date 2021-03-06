<!-- ABOUT THE PROJECT -->
## Acoustic Rendering Pipeline

This repository contains implementations of the acoustic rendering pipeline presented in the paper: Acoustic Rendering Based on a Marching Cubes Algorithm for Geometry Reduction Using Acoustic Material Classification.
Currently, this only supports Windows.

### Usage

The pipeline is executed as an offline procedure for a given scene specified in the `conf.json` file.

* Run the geometry reduction to generate image patches from the scene with `unda.exe`
* Predict acoustic materials using the classifier via `classifier/classify_patches.py` and `classifier/calculate_absorption.py`
* Generate Room Impulse Responses re-running `unda.exe`

## Model Architecture and Weights
The model weights and architecture for acoustic material classification is stored as a Keras model `.h5`, available at this [link](https://drive.google.com/file/d/1e2A-KeJeMctVwWE79GKfqWYyH6x1RhGR/view?usp=sharing).

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.

