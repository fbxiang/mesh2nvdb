#include <Python.h>
#include <string>

std::string mesh2nvdb(float *points, uint32_t n_points, int *triangles,
                      uint32_t n_triangles);

struct PyArray_Descr {
  PyObject_HEAD PyTypeObject *typeobj;
  char kind;
  char type;
  char byteorder;
  char flags;
  int type_num;
  int elsize;
  int alignment;
  struct PyArray_ArrayDescr *subarray;
  PyObject *fields;
  PyObject *names;
  struct PyArray_ArrFuncs *f;
  PyObject *metadata;
  struct NpyAuxData *c_metadata;
  Py_hash_t *hash;
};

struct PyArrayObject {
  PyObject_HEAD char *data;
  int nd;
  ssize_t *dimensions;
  ssize_t *strides;
  PyObject *base;
  PyArray_Descr *descr;
  int flags;
  PyObject *weakreflist;
  /* version dependent private members */
};

static PyObject *mesh2nvdb_mesh2nvdb(PyObject *self, PyObject *args) {
  PyObject *numpy = PyImport_ImportModule("numpy");
  PyObject *ndarray = PyObject_GetAttrString(numpy, "ndarray");
  PyObject *int32 = PyObject_GetAttrString(numpy, "int32");
  PyObject *float32 = PyObject_GetAttrString(numpy, "float32");

  PyObject *vertices, *triangles;
  PyArg_ParseTuple(args, "OO", &vertices, &triangles);
  if (!vertices || !triangles || (PyObject *)Py_TYPE(vertices) != ndarray ||
      (PyObject *)Py_TYPE(triangles) != ndarray) {
    PyErr_SetString(PyExc_ValueError,
                    "invalid arguments: vertices and triangles are np.ndarray");
    return nullptr;
  }

  PyArrayObject *vs = reinterpret_cast<PyArrayObject *>(vertices);
  PyArrayObject *ts = reinterpret_cast<PyArrayObject *>(triangles);

  if (vs->nd != 2 || vs->dimensions[1] != 3 || vs->dimensions[0] == 0) {
    PyErr_SetString(PyExc_ValueError,
                    "invalid arguments: vertices must have shape [N, 3]");
    return nullptr;
  }

  vs = reinterpret_cast<PyArrayObject *>(PyObject_CallMethod(
      PyObject_CallMethod(numpy, "ascontiguousarray", "O", vertices), "astype",
      "O", float32));

  if (ts->nd != 2 || ts->dimensions[1] != 3 || ts->dimensions[0] == 0) {
    PyErr_SetString(PyExc_ValueError,
                    "invalid arguments: triangles must have shape [M, 3]");
    return nullptr;
  }

  ts = reinterpret_cast<PyArrayObject *>(PyObject_CallMethod(
      PyObject_CallMethod(numpy, "ascontiguousarray", "O", triangles), "astype",
      "O", int32));

  auto data = mesh2nvdb(reinterpret_cast<float *>(vs->data), vs->dimensions[0],
                        reinterpret_cast<int *>(ts->data), ts->dimensions[0]);

  PyObject *bytes = PyBytes_FromStringAndSize(data.c_str(), data.length());

  return bytes;
}

static struct PyMethodDef mesh2nvdb_methods[] = {
    {
        "mesh2nvdb",
        mesh2nvdb_mesh2nvdb,
        METH_VARARGS,
        "convert mesh to nvdb grid",
    },
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef mesh2nvdb_module = {.m_base = PyModuleDef_HEAD_INIT,
                                              .m_name = "_C",
                                              .m_methods = mesh2nvdb_methods};

PyMODINIT_FUNC PyInit__C(void) {
  PyObject *m = PyModule_Create(&mesh2nvdb_module);
  return m;
}
