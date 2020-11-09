// Copyright (c) 2019 The Pybind Development Team. All rights reserved.
//
// All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.
//
// Type conversion utilities for pybind11 and absl data structures.
//
// Usage: Just include this file in the .cc file with your bindings and add the
// appropriate dependency. Any functions which take or return the supported
// types will have those types automatically converted.
//
// Supported types:
// - absl::Duration- converted to/from python datetime.timedelta
// - absl::CivilTime- converted to/from python datetime.datetime and from date.
// - absl::Time- converted to/from python datetime.datetime and from date.
// - absl::Span- const value types only.
// - absl::string_view
// - absl::optional- converts absl::nullopt to/from python None, otherwise
//   converts the contained value.
// - absl::flat_hash_map- converts to/from python dict.
// - absl::flat_hash_set- converst to/from python set.
//
// For details, see the README.md.
//
// Author: Ken Oslund

#ifndef PYBIND11_ABSEIL_ABSL_CASTERS_H_
#define PYBIND11_ABSEIL_ABSL_CASTERS_H_

#include <pybind11/cast.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cmath>
#include <cstdint>
#include <exception>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/string_view.h"
#include "absl/time/civil_time.h"
#include "absl/time/time.h"
#include "absl/types/optional.h"
#include "absl/types/span.h"

namespace pybind11 {
namespace detail {

// Helper function to get an int64 attribute.
inline int64 GetInt64Attr(handle src, const char* name) {
  return src.attr(name).cast<int64>();
}

// Given a python date or datetime object, figure out an appropriate
// absl::TimeZone with a fixed offset, or default to the local timezone.
inline absl::TimeZone GetTimeZone(handle src) {
  if (!hasattr(src, "tzinfo")) {
    // datetime.date objects lack this property, so assume local.
    return absl::LocalTimeZone();
  }
  object tzinfo = src.attr("tzinfo");
  if (tzinfo.is_none()) {
    // non-tz aware datetime, again assume local time.
    return absl::LocalTimeZone();
  }
  object utc_offset = tzinfo.attr("utcoffset")(src);
  if (utc_offset.is_none()) {
    return absl::LocalTimeZone();
  }
  int64 offset_seconds = std::lround(
      utc_offset.attr("total_seconds")().cast<double>());
  return absl::FixedTimeZone(offset_seconds);
}

// Convert between absl::Duration and python datetime.timedelta.
template <>
struct type_caster<absl::Duration> {
 public:
  // This macro establishes the name 'absl::Duration' in function signatures
  // and declares a local variable 'value' of type absl::Duration.
  PYBIND11_TYPE_CASTER(absl::Duration, _<absl::Duration>());

  // Conversion part 1 (Python->C++)
  bool load(handle src, bool convert) {
    // Ensure that absl::Duration is converted from a Python datetime.timedelta.
    if (!convert || !hasattr(src, "days") || !hasattr(src, "seconds") ||
        !hasattr(src, "microseconds")) {
      return false;
    }
    value = absl::Hours(24 * GetInt64Attr(src, "days")) +
            absl::Seconds(GetInt64Attr(src, "seconds")) +
            absl::Microseconds(GetInt64Attr(src, "microseconds"));
    return true;
  }

  // Conversion part 2 (C++ -> Python)
  static handle cast(const absl::Duration& src, return_value_policy, handle) {
    absl::Duration remainder;
    int64 secs = absl::IDivDuration(src, absl::Seconds(1), &remainder);
    int64 microsecs = absl::ToInt64Microseconds(remainder);
    auto py_duration_t = module::import("datetime").attr("timedelta");
    auto py_duration =
        py_duration_t(arg("seconds") = secs, arg("microseconds") = microsecs);
    return py_duration.release();
  }
};

// Convert between absl::Time and python datetime.date, datetime.
template <>
struct type_caster<absl::Time> {
 public:
  // This macro establishes the name 'absl::Time' in function signatures
  // and declares a local variable 'value' of type absl::Time.
  PYBIND11_TYPE_CASTER(absl::Time, _<absl::Time>());

  // Conversion part 1 (Python->C++)
  bool load(handle src, bool convert) {
    // Ensure that absl::Duration is converted from a Python datetime.date.
    if (!convert || !hasattr(src, "year") || !hasattr(src, "month") ||
        !hasattr(src, "day")) {
      return false;
    }
    if (hasattr(src, "timestamp")) {
      // python datetime.datetime object
      double timestamp = src.attr("timestamp")().cast<double>();
      int64 as_micros = static_cast<int64>(timestamp * 1e6);
      value = absl::FromUnixMicros(as_micros);
    }
#if PY_MAJOR_VERSION < 3
    else if (hasattr(src, "microsecond")) {  // NOLINT(readability/braces)
      // python datetime.datetime object
      // This doesn't rely on datetime.datetime.timestamp(), which is not
      // available in Python 2.
      auto utc = module::import("dateutil.tz").attr("UTC");
      auto datetime = module_::import("datetime").attr("datetime");
      auto gettz = module::import("dateutil.tz").attr("gettz");

      auto epoch_dt = datetime.attr("fromtimestamp")(0, utc);
      auto tz = (src.attr("tzinfo").is_none()) ? gettz() : src.attr("tzinfo");
      auto src_with_tz = src.attr("replace")("tzinfo"_a = tz);

      double timestamp =
          (src_with_tz - epoch_dt).attr("total_seconds")().cast<double>();
      int64 as_micros = static_cast<int64>(timestamp * 1e6);
      value = absl::FromUnixMicros(as_micros);
    }
#endif
    else {  // NOLINT(readability/braces)
      // python datetime.date object
      absl::CivilDay civil_day(GetInt64Attr(src, "year"),
                               GetInt64Attr(src, "month"),
                               GetInt64Attr(src, "day"));
      value = absl::FromCivil(civil_day, GetTimeZone(src));
    }
    return true;
  }

  // Conversion part 2 (C++ -> Python)
  static handle cast(const absl::Time& src, return_value_policy, handle) {
    // This function truncates fractional microseconds as the python datetime
    // objects cannot support a resolution higher than this.
    auto py_datetime_t = module::import("datetime").attr("datetime");
    auto py_from_timestamp = py_datetime_t.attr("fromtimestamp");
    auto py_timezone_t = module::import("dateutil.tz").attr("gettz");
    auto py_timezone = py_timezone_t(absl::LocalTimeZone().name());
    double as_seconds = static_cast<double>(absl::ToUnixMicros(src)) / 1e6;
    auto py_datetime = py_from_timestamp(as_seconds, "tz"_a = py_timezone);
    return py_datetime.release();
  }
};

template <typename CivilTimeType>
struct absl_civil_time_caster {
 public:
  PYBIND11_TYPE_CASTER(CivilTimeType, _<CivilTimeType>());

  bool load(handle src, bool convert) {
    if (!convert || !hasattr(src, "year") || !hasattr(src, "month") ||
        !hasattr(src, "day")) {
      return false;
    }
    int64 hour = 0, minute = 0, second = 0;
    if (hasattr(src, "hour") && hasattr(src, "minute") &&
        hasattr(src, "second")) {
      hour = GetInt64Attr(src, "hour");
      minute = GetInt64Attr(src, "minute");
      second = GetInt64Attr(src, "second");
    }
    value = CivilTimeType(GetInt64Attr(src, "year"),
                          GetInt64Attr(src, "month"),
                          GetInt64Attr(src, "day"), hour, minute, second);
    return true;
  }

  static handle cast(
      const CivilTimeType& src, return_value_policy, handle) {
    auto py_datetime_t = module::import("datetime").attr("datetime");
    auto py_datetime = py_datetime_t(
        src.year(), src.month(), src.day(),
        src.hour(), src.minute(), src.second());
    return py_datetime.release();
  }
};

template<>
struct type_caster<absl::CivilSecond>
    : public absl_civil_time_caster<absl::CivilSecond> {};

template<>
struct type_caster<absl::CivilMinute>
    : public absl_civil_time_caster<absl::CivilMinute> {};

template<>
struct type_caster<absl::CivilHour>
    : public absl_civil_time_caster<absl::CivilHour> {};

template<>
struct type_caster<absl::CivilDay>
    : public absl_civil_time_caster<absl::CivilDay> {};

template<>
struct type_caster<absl::CivilMonth>
    : public absl_civil_time_caster<absl::CivilMonth> {};

template<>
struct type_caster<absl::CivilYear>
    : public absl_civil_time_caster<absl::CivilYear> {};

// Convert between absl::Span and python sequence types.
//
// TODO(kenoslund): It may be possible to avoid copies in some cases:
// Python to C++: Numpy arrays are contiguous, so we could overlay without copy.
// C++ to Python: Python buffer.
// https://pybind11.readthedocs.io/en/stable/advanced/pycpp/numpy.html#buffer-protocol
template <typename T>
struct type_caster<absl::Span<const T>> {
  // The vector caster does not work with pointers, so neither does this.
  // Making this work should be possible- it would just require writing a
  // vector converter which keeps it's element casters alive (currently they
  // are local variables and are discarded before the value can be used).
  static_assert(!std::is_pointer<T>::value,
                "Spans of pointers are not supported.");

  type_caster() : vector_converter_(), value_(get_vector()) {}
  // Copy and Move constructors need to ensure the span points to the copied
  // or moved vector, not the original one.
  type_caster(const type_caster<absl::Span<const T>>& other)
      : vector_converter_(other.vector_converter_), value_(get_vector()) {}
  type_caster(type_caster<absl::Span<const T>>&& other)
      : vector_converter_(std::move(other.vector_converter_)),
        value_(get_vector()) {}

  static constexpr auto name = _("Span[") + make_caster<T>::name + _("]");

  // We do not allow moving because 1) spans are super lightweight, so there's
  // no advantage to moving and 2) the span cannot exist without the caster,
  // so moving leaves an implicit dependency (while a reference or pointer
  // make that dependency explicit).
  operator absl::Span<const T> *() { return &value_; }
  operator absl::Span<const T> &() { return value_; }
  template <typename T_>
  using cast_op_type = cast_op_type<T_>;

  bool load(handle src, bool convert) {
    if (!vector_converter_.load(src, convert)) return false;
    // std::vector implicitly converted to absl::Span.
    value_ = get_vector();
    return true;
  }

  template <typename CType>
  static handle cast(CType&& src, return_value_policy policy, handle parent) {
    return VectorConverter::cast(src, policy, parent);
  }

 private:
  std::vector<T>& get_vector() {
    return static_cast<std::vector<T>&>(vector_converter_);
  }
  using VectorConverter = make_caster<std::vector<T>>;
  VectorConverter vector_converter_;
  absl::Span<const T> value_;
};

// Convert between absl::flat_hash_map and python dict.
template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
struct type_caster<absl::flat_hash_map<Key, Value, Hash, Equal, Alloc>>
    : map_caster<absl::flat_hash_map<Key, Value, Hash, Equal, Alloc>, Key,
                 Value> {};

// Convert between absl::flat_hash_set and python set.
template <typename Key, typename Hash, typename Equal, typename Alloc>
struct type_caster<absl::flat_hash_set<Key, Hash, Equal, Alloc>>
  : set_caster<absl::flat_hash_set<Key, Hash, Equal, Alloc>, Key> { };

// Convert between absl::string_view and python.
//
// pybind11 supports std::string_view, and absl::string_view is meant to be a
// drop-in replacement for std::string_view, so we can just use the built in
// implementation. This is only needed until absl::string_view becomes an alias
// for std::string_view.
#ifndef ABSL_USES_STD_STRING_VIEW
template <>
struct type_caster<absl::string_view> : string_caster<absl::string_view, true> {
};
#endif

// Convert between absl::optional and python.
//
// pybind11 supports std::optional, and absl::optional is meant to be a
// drop-in replacement for std::optional, so we can just use the built in
// implementation.
#ifndef ABSL_USES_STD_OPTIONAL
template <typename T>
struct type_caster<absl::optional<T>>
    : public optional_caster<absl::optional<T>> {};
template <>
struct type_caster<absl::nullopt_t> : public void_caster<absl::nullopt_t> {};
#endif

}  // namespace detail

// Convert an std::string_view into an absl::string_view. This is only needed
// until absl::string_view becomes an alias for std::string_view.
inline absl::string_view std_to_absl(std::string_view in) {
  return absl::string_view(in.data(), in.length());
}

}  // namespace pybind11

#endif  // PYBIND11_ABSEIL_ABSL_CASTERS_H_
