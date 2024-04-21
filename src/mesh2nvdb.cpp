#define NANOVDB_USE_OPENVDB

#include <iostream>
#include <string>
#include <vector>

#include <nanovdb/util/CreateNanoGrid.h>
#include <nanovdb/util/IO.h>
#include <openvdb/openvdb.h>
#include <openvdb/tools/MeshToVolume.h>

using namespace openvdb;

std::string mesh2nvdb(float *points, uint32_t n_points, int *triangles,
                      uint32_t n_triangles) {
  math::Transform::Ptr xform = math::Transform::createLinearTransform();
  FloatGrid::Ptr sgrid = tools::meshToSignedDistanceField<FloatGrid>(
      *xform, std::vector((Vec3s *)points, ((Vec3s *)points) + n_points),
      std::vector((Vec3I *)triangles, ((Vec3I *)triangles) + n_triangles), {},
      3.0, 3.0);
  auto handle = nanovdb::createNanoGrid(*sgrid);
  auto *dstGrid = handle.grid<float>();
  if (!dstGrid)
    throw std::runtime_error(
        "GridHandle does not contain a grid with value type float");

  std::ostringstream os;
  nanovdb::io::writeGrid(os, handle, nanovdb::io::Codec::NONE);
  return os.str();
}
