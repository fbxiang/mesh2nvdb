from mesh2nvdb import mesh2nvdb
import numpy as np

vertices = np.array(
    [
        [0, 0, 0],
        [1, 0, 0],
        [0, 1, 0],
        [0, 0, 1],
    ]
)
triangles = np.array(
    [
        [0, 2, 1],
        [0, 1, 3],
        [0, 3, 2],
        [1, 2, 3],
    ]
)
nvdb = mesh2nvdb(vertices, triangles)
