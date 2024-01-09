from absl.testing import absltest

from pybind11_abseil.tests import status_testing_no_cpp_eh_pybind
from pybind11_abseil.tests import status_testing_no_cpp_eh_test_lib as test_lib


class _TestModuleMixin:

  def getTestModule(self):  # pylint: disable=invalid-name
    return status_testing_no_cpp_eh_pybind


class StatusReturnTest(test_lib.StatusReturnTest, _TestModuleMixin):
  pass


class StatusOrReturnTest(test_lib.StatusOrReturnTest, _TestModuleMixin):
  pass


class StatusOrPyObjectPtrTest(
    test_lib.StatusOrPyObjectPtrTest, _TestModuleMixin
):
  pass


if __name__ == '__main__':
  absltest.main()
