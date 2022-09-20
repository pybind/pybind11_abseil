#include "pybind11_abseil/utils_pybind11_absl.h"

#include <Python.h>
#include <pybind11/pybind11.h>

#include "absl/strings/string_view.h"

namespace pybind11 {
namespace google {

str decode_utf8_replace(absl::string_view s) {
  PyObject* u = PyUnicode_DecodeUTF8(s.data(), s.size(), "replace");
  if (u == nullptr) {
    throw error_already_set();
  }
  return reinterpret_steal<str>(u);
}

}  // namespace google
}  // namespace pybind11
