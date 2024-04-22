from mesh2nvdb import mesh2nvdb
import numpy as np
import warp as wp

vertices = np.array(
    [
        [-1, -1, -1],
        [1, -1, -1],
        [-1, 1, -1],
        [-1, -1, 1],
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
nvdb = mesh2nvdb(vertices, triangles, 0.01)

wp.init()
wp.set_device(wp.get_preferred_device())
volume = wp.Volume.load_from_nvdb(nvdb)


@wp.kernel
def test_nvdb(v: wp.uint64):
    index = wp.volume_world_to_index(v, vec3(-0.35, -0.35, -0.35))
    print(index)
    print(wp.volume_sample_f(v, index, wp.Volume.LINEAR))

wp.launch(test_nvdb, [1], [volume.id])
wp.synchronize()

