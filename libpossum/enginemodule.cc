#include <Python.h>

static PyObject *
foo (PyObject *self, PyObject *args)
{
  return Py_BuildValue ("i", 12);
}

static PyMethodDef engine_methods[] = {
  { "foo", foo, METH_VARARGS, "Return 12" },
  { NULL }
};

PyMODINIT_FUNC
initengine(void)
{
  Py_InitModule ("engine", engine_methods);
}
