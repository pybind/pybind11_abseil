from absl.testing import absltest

from pybind11_abseil.tests import status_testing_no_cpp_eh_pybind
from pybind11_abseil.tests import status_testing_no_cpp_eh_test_lib as test_lib

_HAS_FUN_SPEC = (
    status_testing_no_cpp_eh_pybind.defined_PYBIND11_HAS_TYPE_CASTER_STD_FUNCTION_SPECIALIZATIONS
)
_FUN_SPEC_NDEF = (
    'PYBIND11_HAS_TYPE_CASTER_STD_FUNCTION_SPECIALIZATIONS is not defined.'
)


class _TestModuleMixin:

  def getTestModule(self):  # pylint: disable=invalid-name
    return status_testing_no_cpp_eh_pybind


@absltest.skipIf(not _HAS_FUN_SPEC, _FUN_SPEC_NDEF)
class StatusReturnTest(test_lib.StatusReturnTest, _TestModuleMixin):
  pass


@absltest.skipIf(not _HAS_FUN_SPEC, _FUN_SPEC_NDEF)
class StatusOrReturnTest(test_lib.StatusOrReturnTest, _TestModuleMixin):
  pass


@absltest.skipIf(not _HAS_FUN_SPEC, _FUN_SPEC_NDEF)
class StatusOrPyObjectPtrTest(
    test_lib.StatusOrPyObjectPtrTest, _TestModuleMixin
):
  pass


if __name__ == '__main__':
  absltest.main()
