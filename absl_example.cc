// Copyright (c) 2019 The Pybind Development Team. All rights reserved.
//
// All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include <pybind11/pybind11.h>

#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "absl/time/civil_time.h"
#include "absl/time/time.h"
#include "absl/types/optional.h"
#include "absl/types/span.h"
#include "pybind11_abseil/absl_casters.h"
#include "pybind11_abseil/absl_numpy_span_caster.h"

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

absl::CivilSecond MakeCivilSecond(double secs) {
  return absl::ToCivilSecond(absl::FromUnixSeconds(static_cast<int64>(secs)),
                             absl::UTCTimeZone());
}

absl::CivilMinute MakeCivilMinute(double secs) {
  return absl::ToCivilMinute(absl::FromUnixSeconds(static_cast<int64>(secs)),
                             absl::UTCTimeZone());
}

absl::CivilHour MakeCivilHour(double secs) {
  return absl::ToCivilHour(absl::FromUnixSeconds(static_cast<int64>(secs)),
                           absl::UTCTimeZone());
}

absl::CivilDay MakeCivilDay(double secs) {
  return absl::ToCivilDay(absl::FromUnixSeconds(static_cast<int64>(secs)),
                          absl::UTCTimeZone());
}

absl::CivilMonth MakeCivilMonth(double secs) {
  return absl::ToCivilMonth(absl::FromUnixSeconds(static_cast<int64>(secs)),
                            absl::UTCTimeZone());
}

absl::CivilYear MakeCivilYear(double secs) {
  return absl::ToCivilYear(absl::FromUnixSeconds(static_cast<int64>(secs)),
                           absl::UTCTimeZone());
}

bool CheckCivilSecond(absl::CivilSecond datetime, double secs) {
  return datetime == MakeCivilSecond(secs);
}

bool CheckCivilMinute(absl::CivilMinute datetime, double secs) {
  return datetime == MakeCivilMinute(secs);
}

bool CheckCivilHour(absl::CivilHour datetime, double secs) {
  return datetime == MakeCivilHour(secs);
}

bool CheckCivilDay(absl::CivilDay datetime, double secs) {
  return datetime == MakeCivilDay(secs);
}

bool CheckCivilMonth(absl::CivilMonth datetime, double secs) {
  return datetime == MakeCivilMonth(secs);
}

bool CheckCivilYear(absl::CivilYear datetime, double secs) {
  return datetime == MakeCivilYear(secs);
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

bool CheckMap(const absl::flat_hash_map<int, int>& map,
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

absl::flat_hash_set<int> MakeSet(const std::vector<int>& values) {
  return absl::flat_hash_set<int>(values.begin(), values.end());
}

bool CheckSet(const absl::flat_hash_set<int>& set,
              const std::vector<int>& values) {
  absl::flat_hash_set<int> check(values.begin(), values.end());
  return set == check;
}

// Non-const Span.
template <typename T>
void FillNonConstSpan(T value, absl::Span<T> output_span) {
  for (auto& i : output_span) {
    i = value;
  }
}

template <typename T>
void DefineNonConstSpan(module* py_m, absl::string_view type_name) {
  py_m->def(absl::StrCat("fill_non_const_span_", type_name).c_str(),
            &FillNonConstSpan<T>, arg("value"), arg("output_span").noconvert());
}

PYBIND11_MODULE(absl_example, m) {
  // absl::Time/Duration bindings.
  m.def("make_duration", &MakeDuration, arg("secs"));
  m.def("check_duration", &CheckDuration, arg("duration"), arg("secs"));
  m.def("make_datetime", &MakeTime, arg("secs"));
  m.def("check_datetime", &CheckDatetime, arg("datetime"), arg("secs"));

  // absl::CivilTime bindings
  m.def("make_civilsecond", &MakeCivilSecond, arg("secs"));
  m.def("check_civilsecond", &CheckCivilSecond, arg("datetime"), arg("secs"));
  m.def("make_civilminute", &MakeCivilMinute, arg("secs"));
  m.def("check_civilminute", &CheckCivilMinute, arg("datetime"), arg("secs"));
  m.def("make_civilhour", &MakeCivilHour, arg("secs"));
  m.def("check_civilhour", &CheckCivilHour, arg("datetime"), arg("secs"));
  m.def("make_civilday", &MakeCivilDay, arg("secs"));
  m.def("check_civilday", &CheckCivilDay, arg("datetime"), arg("secs"));
  m.def("make_civilmonth", &MakeCivilMonth, arg("secs"));
  m.def("check_civilmonth", &CheckCivilMonth, arg("datetime"), arg("secs"));
  m.def("make_civilyear", &MakeCivilYear, arg("secs"));
  m.def("check_civilyear", &CheckCivilYear, arg("datetime"), arg("secs"));

  // absl::Span bindings.
  m.def("check_span", &CheckSpan, arg("span"), arg("values"));
  class_<VectorContainer>(m, "VectorContainer")
      .def(init())
      .def("make_span", &VectorContainer::MakeSpan, arg("values"));

  // non-const absl::Span bindings.
  DefineNonConstSpan<double>(&m, "double");
  DefineNonConstSpan<int>(&m, "int");
  // Wrap a const Span with a non-const Span lambda to avoid copying data.
  m.def(
      "check_span_no_copy",
      [](absl::Span<int32> span, const std::vector<int32>& values) -> bool {
        return CheckSpan(span, values);
      },
      arg("span"), arg("values"));

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

  // absl::flat_hash_set bindings
  m.def("make_set", &MakeSet, arg("values"));
  m.def("check_set", &CheckSet, arg("set"), arg("values"));
}

}  // namespace test
}  // namespace pybind11
