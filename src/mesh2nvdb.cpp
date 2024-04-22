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
                      uint32_t n_triangles, float gridSize) {

  std::vector ps((Vec3s *)points, ((Vec3s *)points) + n_points);
  std::vector ts((Vec3I *)triangles, ((Vec3I *)triangles) + n_triangles);

  Vec3s lower = Vec3s(std::numeric_limits<float>::max(),
                      std::numeric_limits<float>::max(),
                      std::numeric_limits<float>::max());
  for (auto &p : ps) {
    lower.x() = std::min(lower.x(), p.x());
    lower.y() = std::min(lower.y(), p.y());
    lower.z() = std::min(lower.z(), p.z());
  }

  math::Transform::Ptr xform = math::Transform::createLinearTransform(
    Mat4R(
      (double)gridSize, 0., 0., 0.,
      0., (double)gridSize, 0., 0.,
      0., 0., (double)gridSize, 0.,
      lower.x() - 3. * gridSize,  lower.y() - 3. * gridSize, lower.z() - 3. * gridSize, 1.0)
    );

  FloatGrid::Ptr sgrid =
      tools::meshToSignedDistanceField<FloatGrid>(*xform, ps, ts, {}, 3.0, 3.0);
  auto handle = nanovdb::createNanoGrid(*sgrid);
  auto *dstGrid = handle.grid<float>();
  if (!dstGrid)
    throw std::runtime_error(
        "GridHandle does not contain a grid with value type float");

  std::ostringstream os;
  nanovdb::io::writeGrid(os, handle, nanovdb::io::Codec::NONE);
  return os.str();
}
