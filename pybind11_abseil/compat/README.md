Note
----

The code here

* only depends on Python and abseil-cpp.

* It does not depend on pybind11, mainly for compatibility with Google-internal
  policies: use of C++ exception handling is strongly discouraged / banned.

  See also:

  * https://google.github.io/styleguide/cppguide.html#Exceptions
