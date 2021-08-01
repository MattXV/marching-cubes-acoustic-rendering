import pywavefront
import matplotlib.pyplot as plt
from scipy import linalg


ortho = linalg.orth([[0, 64], [64, 0]])

print(ortho)
quit()

EXAMPLE_OBJECT = 'resources/models/Church/Scene/building1.obj'
scene = pywavefront.Wavefront(EXAMPLE_OBJECT)
scene.parse()

vertices = list()
vertices_x = list()
vertices_y = list()
vertices_z = list()
for name, material in scene.materials.items():
    
    i = 0;
    for i in range(len(material.vertices) // 3):
        for j in range(3):
            vertices.append(material.vertices[i * j + j])
            vertices_x.append(material.vertices[i * j])
            vertices_y.append(material.vertices[i * j + 1])
            vertices_z.append(material.vertices[i * j + 2])
        



fig = plt.figure()
ax = plt.axes(projection='3d')
ax.scatter(vertices_x, vertices_y, vertices_z)
plt.show()