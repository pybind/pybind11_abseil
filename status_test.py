"""Tests for google3.third_party.pybind11_abseil.status_casters."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from google3.testing.pybase import googletest
from pybind11_abseil import status
from pybind11_abseil import status_example


class StatusTest(googletest.TestCase):

  def test_pass_status(self):
    test_status = status.Status(status.StatusCode.CANCELLED, 'test')
    self.assertTrue(
        status_example.check_status(test_status, status.StatusCode.CANCELLED))

  def test_return_ok(self):
    # The return_status function should convert an ok status to None.
    self.assertIsNone(status_example.return_status(status.StatusCode.OK))

  def test_return_not_ok(self):
    # The return_status function should convert a non-ok status to an exception.
    with self.assertRaises(status.StatusNotOk) as cm:
      status_example.return_status(status.StatusCode.CANCELLED, 'test')
    self.assertEqual(cm.exception.status.code(), status.StatusCode.CANCELLED)
    self.assertEqual(cm.exception.status.message(), 'test')

  def test_make_ok(self):
    # The make_status function has been set up to return a status object
    # instead of raising an exception (this is done in status_example.cc).
    test_status = status_example.make_status(status.StatusCode.OK)
    self.assertEqual(test_status.code(), status.StatusCode.OK)
    self.assertTrue(test_status.ok())

  def test_make_not_ok(self):
    # The make_status function should always return a status object, even if
    # it is not ok (ie, it should *not* convert it to an exception).
    test_status = status_example.make_status(status.StatusCode.CANCELLED)
    self.assertEqual(test_status.code(), status.StatusCode.CANCELLED)
    self.assertFalse(test_status.ok())

  def test_make_not_ok_manual_cast(self):
    test_status = status_example.make_status_manual_cast(
        status.StatusCode.CANCELLED)
    self.assertEqual(test_status.code(), status.StatusCode.CANCELLED)

  def test_make_status_ref(self):
    result_1 = status_example.make_status_ref(status.StatusCode.OK)
    self.assertEqual(result_1.code(), status.StatusCode.OK)
    result_2 = status_example.make_status_ref(status.StatusCode.CANCELLED)
    self.assertEqual(result_2.code(), status.StatusCode.CANCELLED)
    # result_1 and 2 reference the same value, so they should always be equal.
    self.assertEqual(result_1.code(), result_2.code())

  def test_make_status_ptr(self):
    result_1 = status_example.make_status_ptr(status.StatusCode.OK)
    self.assertEqual(result_1.code(), status.StatusCode.OK)
    result_2 = status_example.make_status_ptr(status.StatusCode.CANCELLED)
    self.assertEqual(result_2.code(), status.StatusCode.CANCELLED)
    # result_1 and 2 reference the same value, so they should always be equal.
    self.assertEqual(result_1.code(), result_2.code())

  def test_canonical_error(self):
    test_status = status.aborted_error('test')
    self.assertEqual(test_status.code(), status.StatusCode.ABORTED)
    self.assertEqual(test_status.message(), 'test')

  def test_member_method(self):
    test_status = status_example.TestClass().make_status(status.StatusCode.OK)
    self.assertEqual(test_status.code(), status.StatusCode.OK)
    test_status = status_example.TestClass().make_status_const(
        status.StatusCode.OK)
    self.assertEqual(test_status.code(), status.StatusCode.OK)

  def test_is_ok(self):
    ok_status = status_example.make_status(status.StatusCode.OK)
    self.assertTrue(status.is_ok(ok_status))
    failure_status = status_example.make_status(status.StatusCode.CANCELLED)
    self.assertFalse(status.is_ok(failure_status))


class StatusOrTest(googletest.TestCase):

  def test_return_value(self):
    self.assertEqual(status_example.return_value_status_or(5), 5)

  def test_return_not_ok(self):
    with self.assertRaises(status.StatusNotOk) as cm:
      status_example.return_failure_status_or(status.StatusCode.NOT_FOUND)
    self.assertEqual(cm.exception.status.code(), status.StatusCode.NOT_FOUND)

  def test_make_not_ok(self):
    self.assertEqual(
        status_example.make_failure_status_or(
            status.StatusCode.CANCELLED).code(), status.StatusCode.CANCELLED)

  def test_make_not_ok_manual_cast(self):
    self.assertEqual(
        status_example.make_failure_status_or_manual_cast(
            status.StatusCode.CANCELLED).code(), status.StatusCode.CANCELLED)

  def test_return_ptr_status_or(self):
    result_1 = status_example.return_ptr_status_or(5)
    self.assertEqual(result_1.value, 5)
    result_2 = status_example.return_ptr_status_or(6)
    self.assertEqual(result_2.value, 6)
    # result_1 and 2 reference the same value, so they should always be equal.
    self.assertEqual(result_1.value, result_2.value)

  def test_return_unique_ptr(self):
    result = status_example.return_unique_ptr_status_or(5)
    self.assertEqual(result.value, 5)

  def test_member_method(self):
    test_status = status_example.TestClass().make_failure_status_or(
        status.StatusCode.ABORTED)
    self.assertEqual(test_status.code(), status.StatusCode.ABORTED)

  def test_is_ok(self):
    ok_result = status_example.return_value_status_or(5)
    self.assertEqual(ok_result, 5)
    self.assertTrue(status.is_ok(ok_result))
    failure_result = status_example.make_failure_status_or(
        status.StatusCode.CANCELLED)
    self.assertFalse(status.is_ok(failure_result))


if __name__ == '__main__':
  googletest.main()
