// Copyright (c) 2019 The Pybind Development Team. All rights reserved.
//
// All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include <pybind11/pybind11.h>

#include <vector>

#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "absl/types/optional.h"
#include "pybind11_abseil/absl_casters.h"

namespace pybind11 {
namespace test {

absl::Duration MakeDuration(double secs) { return absl::Seconds(secs); }

bool CheckDuration(const absl::Duration& duration, double secs) {
  return duration == MakeDuration(secs);
}

absl::Time MakeTime(double secs) {
  int64 int_secs = static_cast<int64>(secs);
  int64 int_microsecs = static_cast<int64>((secs - int_secs) * 1e6);
  return absl::FromUnixSeconds(int_secs) + absl::Microseconds(int_microsecs);
}

bool CheckDatetime(const absl::Time& datetime, double secs) {
  return datetime == MakeTime(secs);
}

bool CheckSpan(absl::Span<const int32> span, const std::vector<int32>& values) {
  if (span.size() != values.size()) return false;
  for (int i = 0; i < span.size(); ++i) {
    if (span[i] != values[i]) return false;
  }
  return true;
}

// Since a span does not own its elements, we must create a class to own them
// and persist beyond the function that constructs the span for testing.
class VectorContainer {
 public:
  absl::Span<const int32> MakeSpan(const std::vector<int32>& values) {
    values_ = values;
    return values_;
  }

 private:
  std::vector<int32> values_;
};

bool CheckStringView(absl::string_view view, const std::string& values) {
  return view == values;
}

// Since a string view does not own its data, we must create a class to own
// them and persist beyond the function that constructs the span for testing.
class StringContainer {
 public:
  absl::string_view MakeStringView(const std::string& values) {
    values_ = values;
    return values_;
  }

 private:
  std::string values_;
};

bool CheckOptional(const absl::optional<int> optional, bool given, int value) {
  if (!given && !optional.has_value()) return true;
  if (given && optional.has_value() && optional.value() == value) return true;
  return false;
}

absl::optional<int> MakeOptional() { return absl::nullopt; }
absl::optional<int> MakeOptional(int value) { return value; }


absl::flat_hash_map<int, int> MakeMap(
    const std::vector<std::pair<int, int>>& keys_and_values) {
  absl::flat_hash_map<int, int> map;
  for (const auto& kvp : keys_and_values) {
    map.insert(kvp);
  }
  return map;
}

bool CheckMap(
    const absl::flat_hash_map<int, int>& map,
    const std::vector<std::pair<int, int>>& keys_and_values) {
  for (const auto& kvp : keys_and_values) {
    auto found = map.find(kvp.first);
    if (found == map.end()) {
      return false;
    }
    if (found->second != kvp.second) {
      return false;
    }
  }
  return true;
}

PYBIND11_MODULE(absl_example, m) {
  // absl::Time/Duration bindings.
  m.def("make_duration", &MakeDuration, arg("secs"));
  m.def("check_duration", &CheckDuration, arg("duration"), arg("secs"));
  m.def("make_datetime", &MakeTime, arg("secs"));
  m.def("check_datetime", &CheckDatetime, arg("datetime"), arg("secs"));

  // absl::Span bindings.
  m.def("check_span", &CheckSpan, arg("span"), arg("values"));
  class_<VectorContainer>(m, "VectorContainer")
      .def(init())
      .def("make_span", &VectorContainer::MakeSpan, arg("values"));

  // absl::string_view bindings.
  m.def("check_string_view", &CheckStringView, arg("view"), arg("values"));
  class_<StringContainer>(m, "StringContainer")
      .def(init())
      .def("make_string_view", &StringContainer::MakeStringView, arg("values"));

  // absl::optional bindings.
  m.def("check_optional", &CheckOptional, arg("optional") = absl::nullopt,
        arg("given") = false, arg("value") = 0);
  m.def("make_optional", (absl::optional<int>(*)()) & MakeOptional);
  m.def("make_optional", (absl::optional<int>(*)(int)) & MakeOptional,
        arg("value"));

  // absl::flat_hash_map bindings
  m.def("make_map", &MakeMap, arg("keys_and_values"));
  m.def("check_map", &CheckMap, arg("map"), arg("keys_and_values"));
}

}  // namespace test
}  // namespace pybind11
