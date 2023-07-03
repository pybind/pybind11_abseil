#ifndef PYBIND11_ABSEIL_CPP_CAPSULE_TOOLS_MAKE_SHARED_PTR_CAPSULE_H_
#define PYBIND11_ABSEIL_CPP_CAPSULE_TOOLS_MAKE_SHARED_PTR_CAPSULE_H_

// Must be first include (https://docs.python.org/3/c-api/intro.html).
#include <Python.h>

#include <cassert>
#include <memory>

namespace pybind11_abseil {
namespace cpp_capsule_tools {

// Returns a capsule owning a dynamically allocated copy of the passed
// shared_ptr, or nullptr if an error occurred.
// If the return value is nullptr, the Python error indicator is set.
template <typename T>
PyObject* MakeSharedPtrCapsule(const std::shared_ptr<T>& sp, const char* name) {
  using sp_t = std::shared_ptr<T>;
  std::unique_ptr<sp_t> sp_heap(new sp_t(sp));  // C++11 compatibility.
  PyObject* cap = PyCapsule_New(
      // Portability note. The function type underlying the pointer-to-function
      // type that results from implicit conversion of this lambda has not got
      // C-language linkage, but seems to work (and this patterns is widely
      // used in the pybind11 sources).
      sp_heap.get(), name, /* PyCapsule_Destructor */ [](PyObject* self) {
        // Fetch (and restore below) existing Python error, if any.
        // This is to not mask errors during teardown.
        PyObject *prev_err_type, *prev_err_value, *prev_err_traceback;
        PyErr_Fetch(&prev_err_type, &prev_err_value, &prev_err_traceback);
        const char* self_name = PyCapsule_GetName(self);
        if (PyErr_Occurred()) {
          // Something is critically wrong with the process if this happens.
          // Skipping deallocation of the owned shared_ptr is most likely
          // completely insignificant in comparison. Intentionally not
          // terminating the process, to not disrupt potentially in-flight
          // error reporting.
          PyErr_Print();
          // Intentionally after PyErr_Print(), to rescue as much information
          // as possible.
          assert(self_name == nullptr);
        } else {
          void* void_ptr = PyCapsule_GetPointer(self, self_name);
          if (PyErr_Occurred()) {
            PyErr_Print();  // See comments above.
            assert(void_ptr == nullptr);
          } else {
            delete static_cast<sp_t*>(void_ptr);
          }
        }
        PyErr_Restore(prev_err_type, prev_err_value, prev_err_traceback);
      });
  if (cap != nullptr) {
    sp_heap.release();
  }
  return cap;
}

}  // namespace cpp_capsule_tools
}  // namespace pybind11_abseil

#endif  // PYBIND11_ABSEIL_CPP_CAPSULE_TOOLS_MAKE_SHARED_PTR_CAPSULE_H_
