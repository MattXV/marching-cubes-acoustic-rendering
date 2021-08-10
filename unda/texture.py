import numpy as np
import glm

WIDTH = 64
HEIGHT = 64

vertex_0 = glm.vec4((0.1, 0.1, 0.0, 1))
vertex_1 = glm.vec4((0.8, 0.5, 0.0, 1))
vertex_2 = glm.vec4((0.2, 0.8, 0.0, 1))

triangle = [vertex_0, vertex_1, vertex_2]

model_matrix = glm.mat4(1.0)
# model_matrix = glm.scale(model_matrix, glm.vec3(0.9))
ortho = glm.ortho(0, 1, 0, 1)

print(ortho * model_matrix * vertex_0)
raster = np.zeros((WIDTH * HEIGHT), np.float32)

