"""Tests for the CallAndCatchPybind11Exceptions wrapper function."""

from google3.testing.pybase import googletest
from pybind11_abseil.tests import status_from_cpp_exc_test_lib


class StatusFromCppExcTest(googletest.TestCase):

  def test_catches_error_already_set(self):
    statuses = status_from_cpp_exc_test_lib.CollectVariousStatuses()

    def clean_source_location_trace(s):
      return s.partition(r'=== Source Location Trace: ===')[0]
    statuses = [clean_source_location_trace(s) for s in statuses]

    self.assertEqual(statuses[0], 'OUT_OF_RANGE: ValueError: test error\n')
    self.assertEqual(statuses[1], 'OUT_OF_RANGE: ValueError: test error 2\n')
    self.assertStartsWith(
        statuses[2],
        'UNKNOWN: RuntimeError: Unable to cast Python instance of type'
        " <class 'int'> to ",
    )
    self.assertEqual(
        statuses[3], 'RESOURCE_EXHAUSTED: test error 3\n'
    )


if __name__ == '__main__':
  googletest.main()
