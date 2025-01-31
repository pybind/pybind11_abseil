# Pybind11 bindings for the Abseil C++ Common Libraries

Github-CI:
| OS \ Build system | Bazel | CMake |
|:------- | :---: | :---: |
| Linux (`amd64`) | [![Build Status][amd64_linux_bazel_status]][amd64_linux_bazel_link] | [![Build Status][amd64_linux_cmake_status]][amd64_linux_cmake_link] |
| MacOS (`amd64`) | [![Build Status][amd64_macos_bazel_status]][amd64_macos_bazel_link] | [![Build Status][amd64_macos_cmake_status]][amd64_macos_cmake_link] |
| MacOS (`arm64`)     | [![Build Status][arm64_macos_bazel_status]][arm64_macos_bazel_link] | [![Build Status][arm64_macos_cmake_status]][arm64_macos_cmake_link] |
| Windows (`amd64`) | [![Build Status][amd64_windows_bazel_status]][amd64_windows_bazel_link] | [![Build Status][amd64_windows_cmake_status]][amd64_windows_cmake_link] |

[amd64_linux_bazel_status]: ./../../actions/workflows/amd64_linux_bazel.yml/badge.svg
[amd64_linux_bazel_link]: ./../../actions/workflows/amd64_linux_bazel.yml
[amd64_macos_bazel_status]: ./../../actions/workflows/amd64_macos_bazel.yml/badge.svg
[amd64_macos_bazel_link]: ./../../actions/workflows/amd64_macos_bazel.yml
[arm64_macos_bazel_status]: ./../../actions/workflows/arm64_macos_bazel.yml/badge.svg
[arm64_macos_bazel_link]: ./../../actions/workflows/arm64_macos_bazel.yml
[amd64_windows_bazel_status]: ./../../actions/workflows/amd64_windows_bazel.yml/badge.svg
[amd64_windows_bazel_link]: ./../../actions/workflows/amd64_windows_bazel.yml

[amd64_linux_cmake_status]: ./../../actions/workflows/amd64_linux_cmake.yml/badge.svg
[amd64_linux_cmake_link]: ./../../actions/workflows/amd64_linux_cmake.yml
[amd64_macos_cmake_status]: ./../../actions/workflows/amd64_macos_cmake.yml/badge.svg
[amd64_macos_cmake_link]: ./../../actions/workflows/amd64_macos_cmake.yml
[arm64_macos_cmake_status]: ./../../actions/workflows/arm64_macos_cmake.yml/badge.svg
[arm64_macos_cmake_link]: ./../../actions/workflows/arm64_macos_cmake.yml
[amd64_windows_cmake_status]: ./../../actions/workflows/amd64_windows_cmake.yml/badge.svg
[amd64_windows_cmake_link]: ./../../actions/workflows/amd64_windows_cmake.yml

## Overview

These adapters make Abseil types work with Pybind11 bindings. For more
information on using Pybind11, see
g3doc/third_party/pybind11/google3_utils/README.md.

To use the converters listed below, just include the header
in the .cc file with your bindings:

```cpp
#include "pybind11_abseil/absl_casters.h"
```

## Installation

pybind11_abseil can be built with Bazel or CMake. Instructions for both are below.

### Bazel

In your BUILD file:

```bzl
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")
```

#### Bzlmod

You can depend on the Bazel module and dependencies via one of the following
commands in your MODULE.bazel:

To depend on a release:

```bzl
bazel_dep(
    name = "pybind11_abseil",
    version = "<selected_version>",
)
```

To depend on floating `master`:

```bzl
http_archive = use_repo_rule("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
  name = "pybind11_bazel",
  strip_prefix = "pybind11_bazel-master",
  urls = ["https://github.com/pybind/pybind11_bazel/archive/refs/heads/master.tar.gz"],
)

http_archive(
  name = "pybind11",
  build_file = "@pybind11_bazel//:pybind11-BUILD.bazel",
  strip_prefix = "pybind11-master",
  urls = ["https://github.com/pybind/pybind11/archive/refs/heads/master.tar.gz"],
)

http_archive(
  name = "pybind11_abseil",
  strip_prefix = "pybind11_abseil-master",
  urls = ["https://github.com/pybind/pybind11_abseil/archive/refs/heads/master.tar.gz"],
)
```

#### WORKSPACE

Bazel workspace support is deprecated and will be removed at a later date.

You will need to depend on `pybind11`, `pybind11_bazel`(see
[doc](https://github.com/pybind/pybind11_bazel#installation), and on
`pybind11_abseil`), e.g.

```bzl
http_archive(
  name = "pybind11_bazel",
  strip_prefix = "pybind11_bazel-master",
  urls = ["https://github.com/pybind/pybind11_bazel/archive/refs/heads/master.tar.gz"],
)

http_archive(
  name = "pybind11",
  build_file = "@pybind11_bazel//:pybind11-BUILD.bazel",
  strip_prefix = "pybind11-master",
  urls = ["https://github.com/pybind/pybind11/archive/refs/heads/master.tar.gz"],
)

http_archive(
  name = "pybind11_abseil",
  strip_prefix = "pybind11_abseil-master",
  urls = ["https://github.com/pybind/pybind11_abseil/archive/refs/heads/master.tar.gz"],
)
```

### CMake

In your project, add a FetchContent for pybind11_abseil. This will also fetch
the appropriate versions of Abseil and pybind11 which your project can use
(eliminating the need for submoduling Abseil or using find_package).

Add the following to your CMakeLists.txt:

```cmake
include(FetchContent)
FetchContent_Declare {
  pybind11_abseil
  GIT_REPOSITORY https://github.com/pybind/pybind11_abseil.git
  GIT_TAG master
}
FetchContent_MakeAvailable(pybind11 abseil-cpp pybind11_abseil)
```

To install the package so that it is accessible from system Python, run cmake
with the flag `-DCMAKE_INSTALL_PYDIR` set to a directory on your PYTHONPATH and
subsequently run `make install`. This also works on projects that include
pybind11_abseil via FetchContent.

## absl::Duration

`absl::Duration` objects are converted to/ from python datetime.timedelta objects.
Therefore, C code cannot mutate any datetime.timedelta objects from python.

## absl::Time

`absl::Time` objects are converted to/from python datetime.datetime objects.
Additionally, datetime.date objects can be converted to `absl::Time` objects.
C code cannot mutate any datetime.datetime objects from python.

Python date objects effectively truncate the time to 0 (i.e., midnight).
Python time objects are not supported because `absl::Time` would implicitly
assume a year, which could be confusing.

### Time zones

Python `datetime` objects include timezone information, while
`absl::Time` does not. When converting from Python to C++, if a timezone is
specified then it will be used to determine the `absl::Time` instant. If no
timezone is specified by the Python `datetime` object, the local timezone is
assumed.

When converting back from C++ to Python, the resultant time will be presented in
the local timezone and the `tzinfo` property set on the `datetime` object to
reflect that. This means that the caller may receive a datetime formatted
in a different timezone to the one they passed in. To handle this safely, the
caller should take care to check the `tzinfo` of any returned `datetime`s.

## absl::CivilTime

`absl::CivilTime` objects are converted to/from Python datetime.datetime
objects. Fractional Python datetime components are truncated when converting to
less granular C++ types, and time zone information is ignored.

## absl::Span

### Loading

Some python types can be loaded (Python->C++) without copying or converting the
list, while some require copying/ converting the list. The non-converting load
methods will be tried first, and, if the span elements are const, the converting
load methods will be tried next.

Arguments cast to a span with *non-const* elements can never be copied/converted.
To prevent an argument cast to a span with *const* elements from being copied or
converted, mark it as `noconvert()` (see go/pybind11-non-converting-arguments).

The following python types can be loaded *without* copying or converting:

- Numpy array (or anything else that supports [buffer protocol](
  https://docs.python.org/3/c-api/buffer.htm)) => `Span<{const or non-const} T>`
  if *all* of the following conditions are satisfied:
  - The buffer is 1-D.
  - T is a numeric type.
  - The array dtype matches T exactly.
  - If T is not const, the buffer allows writing.
  - The stride does not indicate to skip elements or go in reverse order.
- [Opaque](https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html#making-opaque-types) `std::vector<T>` => `Span<{const or non-const} T>`.
  - T can be any type, including converted or pointer types, but must
    match exactly between C++ and python.
  - Opaque vectors are *not* currently compatible with the smart holder.

The following python types must be copied/converted to be loaded:

- Python sequence of elements that require conversion (numbers, strings,
  datetimes, etc) => `Span<const T>`.
  - The elements will be copied/ converted, so that conversion must be legal.
  - T *cannot* be a pointer.
- Python sequence of elements that do *not* require conversion (ie, classes
  wrapped with py::class_) => `Span<const T>` (elements *will* be copied) or
  `Span<{const or non-const} T* const>` (elements will *not* be copied).

Specifically, this conversion will *fail* if any of the following are true:

- `noconvert()` was specified (see go/pybind11-non-converting-arguments).
- The element conversion is not allowed (eg, floating point to integer).
- The sequence is being loaded into a `Span<{non-const} T>` or
  `Span<{const or non-const} T* {non-const}>`.
- The elements require conversion *and* the sequence is being loaded into a
  `Span<T*>` (regardless of any `const`s; the element caster which owns the
  converted value would be destroyed before `load` is complete, resulting in
  dangling references).
- The span is nested (ie, `absl::Span<absl::Span<T>>`, regardless of any `const`s).

Note: These failure conditions only apply to *converted* python types.

### Casting

Spans are cast (C++->Python) with the standard list caster, which always
converts the list. This could be changed in the future (eg, using
[buffer protocol](https://pybind11.readthedocs.io/en/stable/advanced/pycpp/numpy.html#buffer-protocol)
) but generally using spans as return values is not recommended.

## absl::string_view

Supported exactly the same way pybind11 supports `std::string_view`.

## absl::optional

Supported exactly the same way pybind11 supports `std::optional`.

## absl::flat_hash_map and absl::btree_map

Supported exactly the same way pybind11 supports `std::map`.

## absl::flat_hash_set

Supported exactly the same way pybind11 supports `std::set`.

## absl::Status[Or]

To use the Status[Or] casters:

1. Include the header file `pybind11_abseil/status_casters.h`
   in the .cc file with your bindings.
1. Call `pybind11::google::ImportStatusModule();` in your `PYBIND11_MODULE`
   definition.

(For use outside google3:
The path used for the `status` module may be changed by altering the value of
`PYBIND11_ABSEIL_STATUS_MODULE_PATH` defined in `import_status_module.h`.)

By default, an ok status will be converted into `None`, and a non-ok status will
raise a `status.StatusNotOk` exception. This has a `status` attribute which can
be used to access the status object and check the code/ message.

To get a `status.Status` object rather than having an exception thrown, pass
either the `Status` object or a function returning a `Status` to
`pybind11::google::DoNotThrowStatus` before casting or binding. This works with
references and pointers to `absl::Status` objects too.

It isn't possible to specify separate return value policies for a `StatusOr`
object and its payload. Since `StatusOr` is processed and not ever actually
represented in Python, the return value policy applies to the payload. E.g., if
you return a `StatusOr<MyObject*>` (note the `*` is inside the `StatusOr`) with
a take_ownership return val policy and the status is OK (i.e., it has a payload)
, Python will take ownership of that payload and free it when it is garbage
collected.

However, if you return a `StatusOr<MyObject>*` (note: the `*` is outside the
`StatusOr` rather than inside it now) with a `take_ownership` return val policy,
Python does not take ownership of the `StatusOr` and will not free it (because
again, that policy applies to `MyObject`, not `StatusOr`).

See `status_utils.cc` in this directory for details about what methods are
available in wrapped `absl::Status` objects.

Example:

```cpp
#include "pybind11_abseil/status_casters.h"

absl::Status StatusReturningFunction() {
  return absl::Status(...);
}

pybind11::object StatusHandlingFunction() {
  return pybind11::cast(pybind11::google::DoNotThrowStatus(StatusReturningFunction()));
}

PYBIND11_MODULE(test_bindings, m) {
  pybind11::google::ImportStatusModule();

  m.def("return_status", &StatusReturningFunction,
        "Return None if StatusCode is OK, otherwise raise an error.");
  m.def("make_status", google::DoNotThrowStatus(&StatusReturningFunction),
        "Return a wrapped status object without raising an error.");
  m.def("status_handling_function", &StatusHandlingFunction,
        "Same effect as make_status, but cast is done internally.");
};
```

Python:

```python
from pybind11_abseil import status
import test_bindings

my_status = make_status()
if my_status.code():
  ...

try:
  return_status()
except status.StatusNotOk as e:
  print(e.status)
```

### absl::StatusOr

`absl::StatusOr` objects behave exactly like `absl::Status` objects, except:

- There is no support for passing `StatusOr` objects. You can only return them.
- Instead of returning None or a wrapped status with OK, this casts and
  returns the payload when there is no error.

As with `absl::Status`, the default behavior is to throw an error when casting
a non-ok status. You may pass a `StatusOr` object or `StatusOr` returning
function to `pybind11::google::DoNotThrowStatus` in exactly the same way as with
`absl::Status` to change this behavior.

`absl::StatusOr` objects must be returned by value (not reference or pointer).
Why? Because the implementation takes advantage of the fact that python is a
dynamically typed language to cast and return the payload *or* the
`absl::Status` object (or raise an exeception). Python has no concept of a
`absl::StatusOr` object, so it's also impossible to apply the
return_value_policy to a `absl::StatusOr`. Therefore returning a reference or
pointer to a `absl::StatusOr` is meaningless.

Pointers *can* be used as the payload type, and the return_value_policy will
be applied to the payload if the status is OK. However, references cannot be
used as the payload type, because that's a restriction on `absl::StatusOr` in
general, not pybind11 (see https://yaqs/5903163345338368).

This can handle any type of payload that pybind knows about. unique_ptrs (i.e.,
`absl::StatusOr<std::unique_ptr<...>>`) to wrapped classes or structs (i.e., any
type which you created bindings for using `pybind11::class_<...>`) can be used,
but unique_ptrs to converted types (e.g., `int`, `string`, `absl::Time`,
`absl::Duration`, etc.) cannot be used.

### absl::StatusCode

The `status` module provides `pybind11::enum_` bindings for `absl::StatusCode`.
These use python constant style, e.g. `status.StatusCode.OK`,
`status.StatusCode.CANCELLED`, etc.

Warning: Pybind enums are their own type, and will never compare equally to
integers due to being a different type, regardless of their value. In particular
, note that the [status proto](http://google3/util/task/status.proto)
`code` field is an integer, so it will never directly compare as equal to a
`StatusCode`. To fix this, convert an integer to a `StatusCode` or vice-versa.

```python
status_code = 0  # An integer.

if status_code == status.StatusCode.OK:  # Wrong: always evaluates to false.
  ...

if status.StatusCode(status_code) == status.StatusCode.OK:  # Correct.
  ...

if status_code == int(status.StatusCode.OK):  # Also correct.
  ...
```
