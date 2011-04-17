#include <Python.h>

#include "engine.h"

PyMODINIT_FUNC
initengine(void)
{
  possum_py_initmodule ();
}
