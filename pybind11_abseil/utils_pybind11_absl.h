#ifndef PYBIND11_ABSEIL_UTILS_PYBIND11_ABSL_H_
#define PYBIND11_ABSEIL_UTILS_PYBIND11_ABSL_H_

// Note: This is meant to only depend on pybind11 and absl headers.
//       DO NOT add other dependencies.

#include <pybind11/pybind11.h>

#include "absl/strings/string_view.h"

namespace pybind11 {
namespace google {

// To avoid clobbering potentially critical error messages with
// `UnicodeDecodeError`.
str decode_utf8_replace(absl::string_view s);

}  // namespace google
}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_UTILS_PYBIND11_ABSL_H_
