// Copyright (c) 2019-2021 The Pybind Development Team. All rights reserved.
//
// All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <complex>
#include <cstddef>
#include <vector>

#include "absl/container/btree_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "absl/time/civil_time.h"
#include "absl/time/time.h"
#include "absl/types/optional.h"
#include "absl/types/span.h"
#include "pybind11_abseil/absl_casters.h"

namespace pybind11 {
namespace test {

absl::Duration MakeDuration(double secs) { return absl::Seconds(secs); }

absl::Duration MakeInfiniteDuration() { return absl::InfiniteDuration(); }

bool IsInfiniteDuration(const absl::Duration& duration) {
  return duration == absl::InfiniteDuration();
}

bool CheckDuration(const absl::Duration& duration, double secs) {
  return duration == MakeDuration(secs);
}

absl::Time MakeTime(double secs) {
  int64_t microsecs = static_cast<int64_t>(secs * 1e6);
  return absl::FromUnixMicros(microsecs);
}

bool CheckDatetime(const absl::Time& datetime, double secs) {
  return datetime == MakeTime(secs);
}

absl::CivilSecond MakeCivilSecond(double secs) {
  return absl::ToCivilSecond(absl::FromUnixSeconds(static_cast<int64_t>(secs)),
                             absl::UTCTimeZone());
}

absl::CivilMinute MakeCivilMinute(double secs) {
  return absl::ToCivilMinute(absl::FromUnixSeconds(static_cast<int64_t>(secs)),
                             absl::UTCTimeZone());
}

absl::CivilHour MakeCivilHour(double secs) {
  return absl::ToCivilHour(absl::FromUnixSeconds(static_cast<int64_t>(secs)),
                           absl::UTCTimeZone());
}

absl::CivilDay MakeCivilDay(double secs) {
  return absl::ToCivilDay(absl::FromUnixSeconds(static_cast<int64_t>(secs)),
                          absl::UTCTimeZone());
}

absl::CivilMonth MakeCivilMonth(double secs) {
  return absl::ToCivilMonth(absl::FromUnixSeconds(static_cast<int64_t>(secs)),
                            absl::UTCTimeZone());
}

absl::CivilYear MakeCivilYear(double secs) {
  return absl::ToCivilYear(absl::FromUnixSeconds(static_cast<int64_t>(secs)),
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

absl::Duration RoundtripDuration(const absl::Duration& duration) {
  return duration;
}

absl::Time RoundtripTime(const absl::Time& time) { return time; }

absl::TimeZone RoundtripTimeZone(absl::TimeZone timezone) { return timezone; }

// Since a span does not own its elements, we must create a class to own them
// and persist beyond the function that constructs the span for testing.
class VectorContainer {
 public:
  absl::Span<const int> MakeSpan(const std::vector<int>& values) {
    values_ = values;
    return values_;
  }

 private:
  std::vector<int> values_;
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

bool CheckAbslCord(absl::Cord cord, const std::string& values) {
  return cord == values;
}

absl::Cord ReturnAbslCord(const std::string& values) {
  absl::Cord cord(values);
  return cord;
}

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

absl::node_hash_map<int, int> MakeNodeHashMap(
    const std::vector<std::pair<int, int>>& keys_and_values) {
  absl::node_hash_map<int, int> map;
  for (const auto& kvp : keys_and_values) {
    map.insert(kvp);
  }
  return map;
}

bool CheckNodeHashMap(const absl::node_hash_map<int, int>& map,
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

absl::node_hash_set<int> MakeNodeHashSet(const std::vector<int>& values) {
  return absl::node_hash_set<int>(values.begin(), values.end());
}

bool CheckNodeHashSet(const absl::node_hash_set<int>& set,
                      const std::vector<int>& values) {
  absl::node_hash_set<int> check(values.begin(), values.end());
  return set == check;
}

absl::btree_map<int, int> MakeBtreeMap(
    const std::vector<std::pair<int, int>>& keys_and_values) {
  absl::btree_map<int, int> map;
  for (const auto& kvp : keys_and_values) {
    map.insert(kvp);
  }
  return map;
}

bool CheckBtreeMap(const absl::btree_map<int, int>& map,
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

// Span
bool CheckSpan(absl::Span<const int> span, const std::vector<int>& values) {
  if (span.size() != values.size()) return false;
  for (size_t i = 0; i < span.size(); ++i) {
    if (span[i] != values[i]) return false;
  }
  return true;
}

bool CheckSpanCasterCopy(const handle& span, const std::vector<int>& values) {
  pybind11::detail::make_caster<absl::Span<const int>> caster;
  caster = pybind11::detail::load_type<absl::Span<const int>>(span);
  return CheckSpan(pybind11::detail::cast_op<absl::Span<const int>>(caster),
                   values);
}

void FillSpan(int value, absl::Span<int> output_span) {
  for (auto& i : output_span) i = value;
}

template <typename CmplxType, typename NonConstCmplxType =
                                  typename std::remove_const<CmplxType>::type>
NonConstCmplxType SumSpanComplex(absl::Span<CmplxType> input_span) {
  NonConstCmplxType sum = 0;
  for (auto& i : input_span) sum += i;
  return sum;
}

std::string PassSpanPyObjectPtr(absl::Span<PyObject*> input_span) {
  std::string result;
  for (auto& i : input_span) result += str(i);
  return result;
}

std::string PassSpanBool(absl::Span<bool> input_span) {
  std::string result;
  for (const auto& i : input_span) result += (i ? "t" : "f");
  return result;
}

std::string PassSpanConstBool(absl::Span<const bool> input_span) {
  std::string result;
  for (const auto& i : input_span) result += (i ? "T" : "F");
  return result;
}

struct ObjectForSpan {
  explicit ObjectForSpan(int v) : value(v) {}
  int value;
};

void FillObjectPointersSpan(int value,
                            absl::Span<ObjectForSpan* const> output_span) {
  for (ObjectForSpan* item : output_span) item->value = value;
}

void FillObjectSpan(int value, absl::Span<ObjectForSpan> output_span) {
  for (auto& item : output_span) item.value = value;
}

int SumObjectPointersSpan(absl::Span<const ObjectForSpan* const> span) {
  int result = 0;
  for (const ObjectForSpan* item : span) result += item->value;
  return result;
}

int SumObjectSpan(absl::Span<const ObjectForSpan> span) {
  int result = 0;
  for (auto& item : span) result += item.value;
  return result;
}

// absl::variant
struct A {
  int a;
};
struct B {
  int b;
};
typedef absl::variant<A, B> AOrB;

int VariantToInt(AOrB value) {
  if (absl::holds_alternative<A>(value)) {
    return absl::get<A>(value).a;
  } else if (absl::holds_alternative<B>(value)) {
    return absl::get<B>(value).b;
  } else {
    throw std::exception();
  }
}

std::vector<AOrB> IdentityWithCopy(const std::vector<AOrB>& value) {
  return value;
}
std::vector<absl::variant<A*, B*>> Identity(
    const std::vector<absl::variant<A*, B*>>& value) {
  return value;
}

bool CheckVariant(const absl::variant<absl::monostate, int> optional, bool given, int value) {
  if (!given && !absl::holds_alternative<int>(optional)) return true;
  if (given && absl::holds_alternative<int>(optional) && absl::get<int>(optional) == value) return true;
  return false;
}

absl::variant<absl::monostate, int> MakeVariant() { return {}; }
absl::variant<absl::monostate, int> MakeVariant(int value) { return value; }

}  // namespace test
}  // namespace pybind11

PYBIND11_MAKE_OPAQUE(std::vector<pybind11::test::ObjectForSpan>);

namespace pybind11 {
namespace test {

// Demonstration of constness check for span template parameters.
static_assert(std::is_const<const int>::value);
static_assert(
    std::is_const<int* const>::value);  // pointer is const, int is not.
static_assert(std::is_const<const int* const>::value);
static_assert(!std::is_const<int>::value);
static_assert(!std::is_const<int*>::value);
static_assert(
    !std::is_const<const int*>::value);  // int is const, pointer is not.

PYBIND11_MODULE(absl_example, m) {
  m.attr("PYBIND11_HAS_RETURN_VALUE_POLICY_CLIF_AUTOMATIC") =
#if defined(PYBIND11_HAS_RETURN_VALUE_POLICY_CLIF_AUTOMATIC)
      true;
#else
      false;
#endif

  // absl::Time/Duration bindings.
  m.def("make_duration", &MakeDuration, arg("secs"));
  m.def("make_infinite_duration", &MakeInfiniteDuration);
  m.def("is_infinite_duration", &IsInfiniteDuration);
  m.def("check_duration", &CheckDuration, arg("duration"), arg("secs"));
  m.def("make_datetime", &MakeTime, arg("secs"));
  m.def("check_datetime", &CheckDatetime, arg("datetime"), arg("secs"));
  m.def("absl_time_overloads", [](const absl::Time&) { return "absl::Time"; });
  m.def("absl_time_overloads", [](int) {  return "int"; });
  m.def("absl_time_overloads", [](float) { return "float"; });
  m.def("make_infinite_future", []() { return absl::InfiniteFuture(); });
  m.def("is_infinite_future",
        [](const absl::Time& time) { return time == absl::InfiniteFuture(); });
  m.def("make_infinite_past", []() { return absl::InfinitePast(); });
  m.def("is_infinite_past",
        [](const absl::Time& time) { return time == absl::InfinitePast(); });

  m.def("roundtrip_duration", &RoundtripDuration, arg("duration"));
  m.def("roundtrip_time", &RoundtripTime, arg("time"));
  m.def("roundtrip_timezone", &RoundtripTimeZone, arg("timezone"));

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
  m.def("check_span_no_convert", &CheckSpan, arg("span").noconvert(),
        arg("values"));
  m.def("check_span_caster_copy", &CheckSpanCasterCopy, arg("span"),
        arg("values"));
  class_<VectorContainer>(m, "VectorContainer")
      .def(init())
      .def("make_span", &VectorContainer::MakeSpan, arg("values"));
  // Non-const spans can never be converted, so `output_span` could be marked as
  // `noconvert`, but that would be redundant (so test that it is not needed).
  m.def("fill_span", &FillSpan, arg("value"), arg("output_span"));
  m.def("sum_span_complex64", &SumSpanComplex<std::complex<float>>);
  m.def("sum_span_const_complex64", &SumSpanComplex<const std::complex<float>>);
  m.def("sum_span_complex128", &SumSpanComplex<std::complex<double>>);
  m.def("sum_span_const_complex128",
        &SumSpanComplex<const std::complex<double>>, arg("input_span"));
  m.def("pass_span_pyobject_ptr", &PassSpanPyObjectPtr, arg("span"));
  m.def("pass_span_bool", &PassSpanBool, arg("span"));
  m.def("pass_span_const_bool", &PassSpanConstBool, arg("span"));

  // Span of objects.
  class_<ObjectForSpan>(m, "ObjectForSpan")
      .def(init<int>())
      .def_readwrite("value", &ObjectForSpan::value);
  bind_vector<std::vector<ObjectForSpan>>(m, "ObjectVector");
  m.def("sum_object_pointers_span", &SumObjectPointersSpan, arg("span"));
  m.def("sum_object_span", &SumObjectSpan, arg("span"));
  m.def("sum_object_span_no_convert", &SumObjectSpan, arg("span").noconvert());
  m.def("fill_object_pointers_span", &FillObjectPointersSpan, arg("value"),
        arg("output_span"));
  m.def("fill_object_span", &FillObjectSpan, arg("value"), arg("output_span"));

  // absl::string_view bindings.
  m.def("check_string_view", &CheckStringView, arg("view"), arg("values"));
  class_<StringContainer>(m, "StringContainer")
      .def(init())
      .def("make_string_view", &StringContainer::MakeStringView, arg("values"));

  // absl::Cord bindings.
  m.def("check_absl_cord", &CheckAbslCord, arg("view"), arg("values"));
  m.def("return_absl_cord", &ReturnAbslCord, arg("values"));
#if defined(PYBIND11_HAS_RETURN_VALUE_POLICY_CLIF_AUTOMATIC)
  m.def("return_absl_cord_clif_automatic", [](const std::string& values) {
    return cast(ReturnAbslCord(values), return_value_policy::_clif_automatic);
  });
#endif

  // absl::optional bindings.
  m.def("check_optional", &CheckOptional, arg("optional") = absl::nullopt,
        arg("given") = false, arg("value") = 0);
  m.def("make_optional", (absl::optional<int>(*)()) & MakeOptional);
  m.def("make_optional", (absl::optional<int>(*)(int)) & MakeOptional,
        arg("value"));

  // absl::flat_hash_map bindings
  m.def("make_map", &MakeMap, arg("keys_and_values"));
  m.def("check_map", &CheckMap, arg("map"), arg("keys_and_values"));

  // absl::node_hash_map bindings
  m.def("make_node_hash_map", &MakeNodeHashMap, arg("keys_and_values"));
  m.def("check_node_hash_map", &CheckNodeHashMap, arg("map"),
        arg("keys_and_values"));

  // absl::flat_hash_set bindings
  m.def("make_set", &MakeSet, arg("values"));
  m.def("check_set", &CheckSet, arg("set"), arg("values"));

  // absl::btree_map bindings
  m.def("make_btree_map", &MakeBtreeMap, arg("keys_and_values"));
  m.def("check_btree_map", &CheckBtreeMap, arg("map"), arg("keys_and_values"));

  // absl::node_hash_set bindings
  m.def("make_node_hash_set", &MakeNodeHashSet, arg("values"));
  m.def("check_node_hash_set", &CheckNodeHashSet, arg("set"), arg("values"));

  // absl::variant
  class_<A>(m, "A").def(init<int>()).def_readonly("a", &A::a);
  class_<B>(m, "B").def(init<int>()).def_readonly("b", &B::b);
  m.def("VariantToInt", &VariantToInt);
  m.def("Identity", &Identity);
  m.def("IdentityWithCopy", &IdentityWithCopy);

  m.def("check_variant", &CheckVariant, arg("optional") = absl::variant<absl::monostate, int>{},
        arg("given") = false, arg("value") = 0);
  m.def("make_variant", (absl::variant<absl::monostate, int>(*)()) & MakeVariant);
  m.def("make_variant", (absl::variant<absl::monostate, int>(*)(int)) & MakeVariant,
        arg("value"));
}

}  // namespace test
}  // namespace pybind11
