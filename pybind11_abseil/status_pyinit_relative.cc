#include <Python.h>

extern "C" PyObject*
GooglePyInit_google3_third__party_pybind11__abseil_status();

extern "C" PyObject* PyInit_status() {
  return GooglePyInit_google3_third__party_pybind11__abseil_status();
}
