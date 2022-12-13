#include <Python.h>

extern "C" PyObject*
GooglePyInit_google3_third__party_pybind11__abseil_status();

// Required only for OSS, but also compatible with the Google toolchain.
#if defined(WIN32) || defined(_WIN32)
#define LOCALDEFINE_DLLEXPORT __declspec(dllexport)
#else
#define LOCALDEFINE_DLLEXPORT __attribute__((visibility("default")))
#endif

extern "C" LOCALDEFINE_DLLEXPORT PyObject* PyInit_status() {
  return GooglePyInit_google3_third__party_pybind11__abseil_status();
}
