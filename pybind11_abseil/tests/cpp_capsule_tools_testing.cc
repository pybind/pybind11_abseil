// Copyright (c) 2023 The Pybind Development Team. All rights reserved.
//
// All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#if true  // go/pybind11_include_order
#include <pybind11/pybind11.h>
#endif

#include <memory>

#include "absl/status/statusor.h"
#include "pybind11_abseil/cpp_capsule_tools/make_shared_ptr_capsule.h"
#include "pybind11_abseil/cpp_capsule_tools/raw_ptr_from_capsule.h"
#include "pybind11_abseil/cpp_capsule_tools/shared_ptr_from_capsule.h"

PYBIND11_MODULE(cpp_capsule_tools_testing, m) {
  namespace py = pybind11;
  namespace cpp_capsule_tools = pybind11_abseil::cpp_capsule_tools;

  m.def("make_bad_capsule", [](bool pass_name) {
    // https://docs.python.org/3/c-api/capsule.html:
    // The pointer argument may not be NULL.
    int dummy_pointee[] = {};  // This will become a dangling pointer when this
    // function returns: We don't want the pointer to be used. Hopefully if it
    // is used unintentionally, one of the sanitizers will flag it.
    return py::capsule(static_cast<void*>(dummy_pointee),
                       pass_name ? "NotGood" : nullptr);
  });

  m.def("make_raw_ptr_capsule", []() {
    static int any_int = 890352;
    return py::capsule(&any_int, "type:int");
  });

  m.def("get_int_from_raw_ptr_capsule",
        [](py::handle py_obj, bool enable_method) {
          absl::StatusOr<int*> status_or_raw_ptr =
              cpp_capsule_tools::RawPtrFromCapsule<int>(
                  py_obj.ptr(), "type:int",
                  (enable_method ? "get_capsule" : nullptr));
          if (!status_or_raw_ptr.ok()) {
            return status_or_raw_ptr.status().ToString();
          }
          return std::to_string(*status_or_raw_ptr.value());
        });

  m.def("make_shared_ptr_capsule", []() {
    return py::reinterpret_steal<py::capsule>(
        cpp_capsule_tools::MakeSharedPtrCapsule(std::make_shared<int>(906069),
                                                "type:shared_ptr<int>"));
  });

  m.def("get_int_from_shared_ptr_capsule",
        [](py::handle py_obj, bool enable_method) {
          using sp_t = std::shared_ptr<int>;
          absl::StatusOr<sp_t> status_or_shared_ptr =
              cpp_capsule_tools::SharedPtrFromCapsule<int>(
                  py_obj.ptr(), "type:shared_ptr<int>",
                  (enable_method ? "get_capsule" : nullptr));
          if (!status_or_shared_ptr.ok()) {
            return status_or_shared_ptr.status().ToString();
          }
          return std::to_string(*status_or_shared_ptr.value());
        });
}
