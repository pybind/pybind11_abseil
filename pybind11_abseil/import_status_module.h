#ifndef PYBIND11_ABSEIL_IMPORT_STATUS_MODULE_H_
#define PYBIND11_ABSEIL_IMPORT_STATUS_MODULE_H_

#include <pybind11/pybind11.h>

// The value of PYBIND11_ABSEIL_STATUS_MODULE_PATH will be different depending
// on whether this is being used inside or outside of google3. The value used
// inside of google3 is defined here. Outside of google3, change this value by
// passing "-DPYBIND11_ABSEIL_STATUS_MODULE_PATH=..." on the commandline.
#ifndef PYBIND11_ABSEIL_STATUS_MODULE_PATH
#define PYBIND11_ABSEIL_STATUS_MODULE_PATH \
  pybind11_abseil.status
#endif

namespace pybind11 {
namespace google {

// Imports the bindings for the status types. This is meant to only be called
// from a PYBIND11_MODULE definition. The Python GIL must be held when calling
// this function (enforced).
// TODO(b/225205409): Remove bypass_regular_import.
// bypass_regular_import is deprecated and can only be false (enforced).
module_ ImportStatusModule(bool bypass_regular_import = false);

}  // namespace google
}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_IMPORT_STATUS_MODULE_H_
