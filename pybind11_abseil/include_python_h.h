#ifndef PYBIND11_ABSEIL_INCLUDE_PYTHON_H_H_
#define PYBIND11_ABSEIL_INCLUDE_PYTHON_H_H_

// Includes <Python.h>. Like <Python.h>, include this before any standard
// headers.
//
// With MSVC, `_DEBUG` (set in debug builds) makes <Python.h> link against the
// debug Python library (e.g. python312_d.lib), which regular Python
// installations do not include. Hide it while including <Python.h>, like
// pybind11/conduit/wrap_include_python_h.h does. That header is not used here
// because it was added in pybind11 v3.0.0, and the CMake and WORKSPACE builds
// still use pybind11 v2.13.6.
//
// Files that include this header put their system headers after it, so those
// includes need `// NOLINT(build/include_order)`.

#if defined(_MSC_VER) && defined(_DEBUG) && !defined(Py_DEBUG)
// Workaround for a VS 2022 issue, see
// https://github.com/pybind/pybind11/pull/3497.
#include <yvals.h>
#if _MSVC_STL_VERSION >= 143
#include <crtdefs.h>
#endif
#define PYBIND11_ABSEIL_DEBUG_MARKER
#undef _DEBUG
#endif

#include <Python.h>  // IWYU pragma: export

#if defined(PYBIND11_ABSEIL_DEBUG_MARKER)
#define _DEBUG 1
#undef PYBIND11_ABSEIL_DEBUG_MARKER
#endif

#endif  // PYBIND11_ABSEIL_INCLUDE_PYTHON_H_H_
