#include <Python.h>

extern "C" PyObject*
GooglePyInit_google3_third__party_pybind11__abseil_ok__status__singleton();

// Required only for OSS, but also compatible with the Google toolchain.
#if defined(WIN32) || defined(_WIN32)
#define LOCALDEFINE_DLLEXPORT __declspec(dllexport)
#else
#define LOCALDEFINE_DLLEXPORT __attribute__((visibility("default")))
#endif

extern "C" LOCALDEFINE_DLLEXPORT PyObject* PyInit_ok_status_singleton() {
  // NOLINTNEXTLINE(whitespace/line_length)
  return GooglePyInit_google3_third__party_pybind11__abseil_ok__status__singleton();
}
