#ifndef PYBIND11_ABSEIL_DISPLAY_SOURCE_LOCATION_IN_PYTHON_H_
#define PYBIND11_ABSEIL_DISPLAY_SOURCE_LOCATION_IN_PYTHON_H_

// This file exists to simplify adding C++ source location to python StatusNotOk
// traces. See: b/266066084 for details.

#include "absl/status/status.h"
#include "util/task/status_builder.h"

namespace pybind11 {
namespace google {

// Return true if the status was set with DisplaySourceLocationInPython.
bool HasDisplaySourceLocationInPython(absl::Status s);
bool HasDoNotDisplaySourceLocationInPython(absl::Status s);

// Annotate the Status to display the c++ SourceLocation in python.
absl::Status DisplaySourceLocationInPython(absl::Status s);
absl::Status DoNotDisplaySourceLocationInPython(absl::Status s);

// Annotate the StatusBuilder to display the c++ SourceLocation in python.
util::StatusBuilder DisplaySourceLocationInPython(util::StatusBuilder sb);
util::StatusBuilder DoNotDisplaySourceLocationInPython(util::StatusBuilder sb);

// Annotate the StatusOr<T> to display the c++ SourceLocation in python.
template<typename StatusOrT>
StatusOrT DisplaySourceLocationInPython(StatusOrT&& s_or_t) {
  if (s_or_t.ok()) {
    return s_or_t;
  }
  absl::Status status_with_payload = DisplaySourceLocationInPython(
      s_or_t.status());
  return StatusOrT{status_with_payload};
}

template<typename StatusOrT>
StatusOrT DoNotDisplaySourceLocationInPython(StatusOrT&& s_or_t) {
  if (s_or_t.ok()) {
    return s_or_t;
  }
  absl::Status status_with_payload = DoNotDisplaySourceLocationInPython(
      s_or_t.status());
  return StatusOrT{status_with_payload};
}

}  // namespace google
}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_DISPLAY_SOURCE_LOCATION_IN_PYTHON_H_
