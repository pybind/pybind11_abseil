// Type conversion utilities for pybind11 and absl::Status/StatusOr.
//
// Usage:
// 1) Include this file in the .cc file with your bindings.
// 2) Call `pybind11::google::ImportStatusModule()` in your PYBIND11_MODULE
//    definition.
//
// Supported types:
// - absl::Status- converts a non-ok return status into a python exception.
//   Can be passed as an argument too if you import the status pybind module.
// - absl::StatusOr- converts a non-ok return status into a python exception,
//   otherwise converts/returns the payload. Can only be used as a return value.
//
// For details, see the README.md.
//
// Author: Ken Oslund (kenoslund@)

// This header declares template specializations, and thus its use may
// not be visible to inclusion analysis tools, but we must retain it.
// IWYU pragma: always_keep

#ifndef PYBIND11_ABSEIL_STATUS_CASTERS_H_
#define PYBIND11_ABSEIL_STATUS_CASTERS_H_

#include "pybind11_abseil/status_caster.h"  // IWYU pragma: export
#include "pybind11_abseil/statusor_caster.h"  // IWYU pragma: export
#include "pybind11_abseil/import_status_module.h"  // IWYU pragma: export

#endif  // PYBIND11_ABSEIL_STATUS_CASTERS_H_
