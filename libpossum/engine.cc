#include <Python.h>

#include "engine.h"

PyObject *
possum_py_foo (PyObject *self, PyObject *args)
{
  return Py_BuildValue ("i", 12);
}

static PyMethodDef engine_methods[] = {
  { "foo", possum_py_foo, METH_VARARGS, "Return 12" },
  { NULL }
};

void
possum_py_initmodule ()
{
  Py_InitModule ("engine", engine_methods);
}
