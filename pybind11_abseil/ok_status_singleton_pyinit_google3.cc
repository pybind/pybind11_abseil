#include <Python.h>

#include "pybind11_abseil/ok_status_singleton_lib.h"

namespace {

static PyObject* wrapOkStatusSingleton(PyObject*, PyObject*) {
  return pybind11_abseil::PyOkStatusSingleton();
}

static PyMethodDef ThisMethodDef[] = {
    {"OkStatusSingleton", wrapOkStatusSingleton, METH_NOARGS,
     "OkStatusSingleton() -> capsule"},
    {}};

static struct PyModuleDef ThisModuleDef = {
    PyModuleDef_HEAD_INIT,  // m_base
    "ok_status_singleton",  // m_name
    nullptr,                // m_doc
    -1,                     // m_size
    ThisMethodDef,          // m_methods
    nullptr,                // m_slots
    nullptr,                // m_traverse
    nullptr,                // m_clear
    nullptr                 // m_free
};

}  // namespace

extern "C" PyObject*
GooglePyInit_google3_third__party_pybind11__abseil_ok__status__singleton() {
  return PyModule_Create(&ThisModuleDef);
}
