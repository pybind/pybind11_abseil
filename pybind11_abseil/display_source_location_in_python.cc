#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "util/task/status_builder.h"

namespace pybind11 {
namespace google {
namespace {

constexpr absl::string_view kDisplay = "1";
constexpr absl::string_view kDoNotDisplay = "0";
constexpr absl::string_view kDisplaySourceLocationInPython =
    "pybind11_abseil_display_source_location";

}  // namespace

bool HasDisplaySourceLocationInPython(absl::Status s) {
  auto optional_payload = s.GetPayload(kDisplaySourceLocationInPython);
  return optional_payload.has_value() &&
      optional_payload.value() == kDisplay;
}

bool HasDoNotDisplaySourceLocationInPython(absl::Status s) {
  auto optional_payload = s.GetPayload(kDisplaySourceLocationInPython);
  return optional_payload.has_value() &&
      optional_payload.value() == kDoNotDisplay;
}

absl::Status DisplaySourceLocationInPython(absl::Status s) {
  s.SetPayload(kDisplaySourceLocationInPython, absl::Cord(kDisplay));
  return s;
}

absl::Status DoNotDisplaySourceLocationInPython(absl::Status s) {
  s.SetPayload(kDisplaySourceLocationInPython, absl::Cord(kDoNotDisplay));
  return s;
}

absl::StatusBuilder DisplaySourceLocationInPython(absl::StatusBuilder sb) {
  return sb.SetPayload(kDisplaySourceLocationInPython, absl::Cord(kDisplay));
}

absl::StatusBuilder DoNotDisplaySourceLocationInPython(absl::StatusBuilder sb) {
  return sb.SetPayload(kDisplaySourceLocationInPython,
                       absl::Cord(kDoNotDisplay));
}

}  // namespace google
}  // namespace pybind11
