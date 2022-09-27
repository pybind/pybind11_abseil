"""Tests for google3.third_party.pybind11_abseil.status_casters."""

import pickle

from absl.testing import absltest
from absl.testing import parameterized
from pybind11_abseil import status
from pybind11_abseil.tests import status_example


def docstring_signature(f):
  """Returns the first line from a docstring - the signature for a function."""
  return f.__doc__.split('\n')[0]


class BadCapsule:

  def __init__(self, pass_name):
    self.pass_name = pass_name

  def as_absl_Status(self):  # pylint: disable=invalid-name
    return status_example.make_bad_capsule(self.pass_name)


class NotACapsule:

  def __init__(self, not_a_capsule):
    self.not_a_capsule = not_a_capsule

  def as_absl_Status(self):  # pylint: disable=invalid-name
    return self.not_a_capsule


class StatusCodeTest(absltest.TestCase):

  def test_status_code_from_int_valid(self):
    self.assertEqual(status.StatusCodeFromInt(13), status.StatusCode.INTERNAL)

  def test_status_code_from_int_invalid(self):
    with self.assertRaises(ValueError) as ctx:
      status.StatusCodeFromInt(9876)
    self.assertEqual(
        str(ctx.exception), 'code_int=9876 is not a valid absl::StatusCode')

  def test_status_code_as_int(self):
    self.assertEqual(status.StatusCodeAsInt(status.StatusCode.UNAVAILABLE), 14)


class StatusTest(parameterized.TestCase):

  def test_pass_status(self):
    test_status = status.Status(status.StatusCode.CANCELLED, 'test')
    self.assertTrue(
        status_example.check_status(test_status, status.StatusCode.CANCELLED))

  def test_return_status_return_type_from_doc(self):
    self.assertEndsWith(
        docstring_signature(status_example.return_status), ' -> None')

  def test_return_ok(self):
    # The return_status function should convert an ok status to None.
    self.assertIsNone(status_example.return_status(status.StatusCode.OK))

  def test_return_not_ok(self):
    # The return_status function should convert a non-ok status to an exception.
    with self.assertRaises(status.StatusNotOk) as cm:
      status_example.return_status(status.StatusCode.CANCELLED, 'test')
    self.assertEqual(cm.exception.status.code(), status.StatusCode.CANCELLED)
    self.assertEqual(cm.exception.status.message(), 'test')
    self.assertEqual(cm.exception.code, int(status.StatusCode.CANCELLED))
    self.assertEqual(cm.exception.message, 'test')

  def test_build_status_not_ok_enum(self):
    e = status.BuildStatusNotOk(status.StatusCode.INVALID_ARGUMENT, 'Msg enum.')
    self.assertEqual(e.status.code(), status.StatusCode.INVALID_ARGUMENT)
    self.assertEqual(e.code, int(status.StatusCode.INVALID_ARGUMENT))
    self.assertEqual(e.message, 'Msg enum.')

  def test_build_status_not_ok_int(self):
    with self.assertRaises(TypeError) as cm:
      status.BuildStatusNotOk(1, 'Msg int.')  # pytype: disable=wrong-arg-types
    self.assertIn('incompatible function arguments', str(cm.exception))

  def test_status_not_ok_status(self):
    e = status.StatusNotOk(status.Status(status.StatusCode.CANCELLED, 'Cnclld'))
    self.assertEqual(e.code, int(status.StatusCode.CANCELLED))
    self.assertEqual(e.message, 'Cnclld')

  def test_status_nok_ok_str(self):
    with self.assertRaises(AttributeError) as cm:
      status.StatusNotOk('')
    self.assertEqual(str(cm.exception), "'str' object has no attribute 'ok'")

  def test_status_nok_ok_none(self):
    with self.assertRaises(AssertionError) as cm:
      status.StatusNotOk(None)
    self.assertEqual(str(cm.exception), '')

  def test_return_not_ok_twice(self):
    # Each exception is a different instance with different messages.
    with self.assertRaises(status.StatusNotOk) as cm1:
      status_example.return_status(status.StatusCode.CANCELLED, 'test1')
    with self.assertRaises(status.StatusNotOk) as cm2:
      status_example.return_status(status.StatusCode.CANCELLED, 'test2')
    self.assertEqual(cm1.exception.status.message(), 'test1')
    self.assertEqual(cm2.exception.status.message(), 'test2')

  def test_return_not_ok_catch_with_alias(self):
    # Catch as status_example.StatusNotOk, an alias of status.StatusNotOk.
    with self.assertRaises(status_example.StatusNotOk) as cm:
      status_example.return_status(status.StatusCode.CANCELLED, 'test')
    self.assertEqual(cm.exception.status.code(), status.StatusCode.CANCELLED)
    self.assertEqual(cm.exception.status.message(), 'test')

  def test_return_not_ok_catch_as_generic_exception(self):
    # Catch as a generic Exception, the base type of StatusNotOk.
    with self.assertRaises(Exception):
      status_example.return_status(status.StatusCode.CANCELLED, 'test')

  def test_make_status_return_type_from_doc(self):
    self.assertRegex(
        docstring_signature(status_example.make_status), r' -> .*\.Status')

  def test_make_ok(self):
    # The make_status function has been set up to return a status object
    # instead of raising an exception (this is done in status_example.cc).
    test_status = status_example.make_status(status.StatusCode.OK)
    self.assertTrue(test_status.ok())
    self.assertEqual(test_status.code(), status.StatusCode.OK)
    self.assertEqual(test_status.code_int(), 0)

  def test_make_not_ok(self):
    # The make_status function should always return a status object, even if
    # it is not ok (ie, it should *not* convert it to an exception).
    test_status = status_example.make_status(status.StatusCode.CANCELLED)
    self.assertFalse(test_status.ok())
    self.assertEqual(test_status.code(), status.StatusCode.CANCELLED)
    self.assertEqual(test_status.code_int(), 1)

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

  def test_repr(self):
    any_status = status_example.make_status(status.StatusCode.DATA_LOSS)
    self.assertRegex(repr(any_status), r'<.*\.status.Status object at .*>')

  def test_ok_to_string(self):
    ok_status = status_example.make_status(status.StatusCode.OK)
    self.assertEqual(ok_status.to_string(), 'OK')
    self.assertEqual(str(ok_status), 'OK')

  def test_canonical_error_to_string(self):
    test_status = status.aborted_error('test')
    self.assertEqual(test_status.to_string(), 'ABORTED: test')
    self.assertEqual(str(test_status), 'ABORTED: test')

  def test_create_ok_status(self):
    ok_status = status.Status.OkStatus()
    self.assertEqual(ok_status.to_string(), 'OK')
    self.assertEqual(ok_status.raw_code(), 0)
    self.assertEqual(ok_status.CanonicalCode(), 0)
    self.assertEqual(ok_status.error_message(), '')
    self.assertIsNone(ok_status.IgnoreError())

  def test_error_message_malformed_utf8(self):
    malformed_utf8 = b'\x80'
    stx80 = status.invalid_argument_error(malformed_utf8)
    self.assertEqual(stx80.message(), '�')
    self.assertEqual(stx80.message_bytes(), malformed_utf8)
    self.assertEqual(stx80.error_message(), '�')
    self.assertEqual(stx80.to_string(), 'INVALID_ARGUMENT: �')
    self.assertEqual(str(stx80), 'INVALID_ARGUMENT: �')
    e = status.StatusNotOk(stx80)
    self.assertEqual(str(e), 'INVALID_ARGUMENT: �')

  def test_raw_code_ne_code(self):
    st500 = status_example.status_from_int_code(500, 'Not a canonical code.')
    self.assertEqual(st500.raw_code(), 500)
    self.assertEqual(st500.code(), status.StatusCode.UNKNOWN)

  def test_payload_management_apis(self):
    st = status.Status(status.StatusCode.CANCELLED, '')
    self.assertEqual(st.AllPayloads(), ())
    st.SetPayload('Url1', 'Payload1')
    self.assertEqual(st.AllPayloads(), ((b'Url1', b'Payload1'),))
    st.SetPayload('Url0', 'Payload0')
    self.assertEqual(st.AllPayloads(),
                     ((b'Url0', b'Payload0'), (b'Url1', b'Payload1')))
    st.SetPayload('Url2', 'Payload2')
    self.assertEqual(st.AllPayloads(),
                     ((b'Url0', b'Payload0'), (b'Url1', b'Payload1'),
                      (b'Url2', b'Payload2')))
    st.SetPayload('Url2', 'Payload2B')
    self.assertEqual(st.AllPayloads(),
                     ((b'Url0', b'Payload0'), (b'Url1', b'Payload1'),
                      (b'Url2', b'Payload2B')))
    self.assertTrue(st.ErasePayload('Url1'))
    self.assertEqual(st.AllPayloads(),
                     ((b'Url0', b'Payload0'), (b'Url2', b'Payload2B')))
    self.assertFalse(st.ErasePayload('Url1'))
    self.assertEqual(st.AllPayloads(),
                     ((b'Url0', b'Payload0'), (b'Url2', b'Payload2B')))
    self.assertFalse(st.ErasePayload('UrlNeverExisted'))
    self.assertEqual(st.AllPayloads(),
                     ((b'Url0', b'Payload0'), (b'Url2', b'Payload2B')))
    self.assertTrue(st.ErasePayload('Url0'))
    self.assertEqual(st.AllPayloads(), ((b'Url2', b'Payload2B'),))
    self.assertTrue(st.ErasePayload('Url2'))
    self.assertEqual(st.AllPayloads(), ())
    self.assertFalse(st.ErasePayload('UrlNeverExisted'))
    self.assertEqual(st.AllPayloads(), ())

  def testDunderEqAndDunderHash(self):
    s0 = status.Status(status.StatusCode.CANCELLED, 'A')
    sb = status.Status(status.StatusCode.CANCELLED, 'A')
    sp = status.Status(status.StatusCode.CANCELLED, 'A')
    sp.SetPayload('Url1p', 'Payload1p')
    sc = status.Status(status.StatusCode.UNKNOWN, 'A')
    sm = status.Status(status.StatusCode.CANCELLED, 'B')
    sx = status.Status(status.StatusCode.UNKNOWN, 'B')

    self.assertTrue(bool(s0 == s0))  # pylint: disable=comparison-with-itself
    self.assertTrue(bool(s0 == sb))
    self.assertFalse(bool(s0 == sp))
    self.assertFalse(bool(s0 == sc))
    self.assertFalse(bool(s0 == sm))
    self.assertFalse(bool(s0 == sx))
    self.assertFalse(bool(s0 == 'AnyOtherType'))

    self.assertEqual(hash(sb), hash(s0))
    self.assertEqual(hash(sp), hash(s0))  # Payload ignored intentionally.
    self.assertNotEqual(hash(sc), hash(s0))
    self.assertNotEqual(hash(sm), hash(s0))
    self.assertNotEqual(hash(sx), hash(s0))

    st_set = {s0}
    self.assertLen(st_set, 1)
    st_set.add(sb)
    self.assertLen(st_set, 1)
    st_set.add(sp)
    self.assertLen(st_set, 2)
    st_set.add(sc)
    self.assertLen(st_set, 3)
    st_set.add(sm)
    self.assertLen(st_set, 4)
    st_set.add(sx)
    self.assertLen(st_set, 5)

  @parameterized.parameters(0, 1, 2)
  def test_pickle(self, payload_size):
    orig = status.Status(status.StatusCode.CANCELLED, 'Cucumber.')
    expected_all_payloads = []
    for i in range(payload_size):
      type_url = f'Url{i}'
      payload = f'Payload{i}'
      orig.SetPayload(type_url, payload)
      expected_all_payloads.append((type_url.encode(), payload.encode()))
    expected_all_payloads = tuple(expected_all_payloads)

    # Redundant with other tests, but here to reassure that the preconditions
    # for the tests below to be meaningful are met.
    self.assertEqual(orig.code(), status.StatusCode.CANCELLED)
    self.assertEqual(orig.message_bytes(), b'Cucumber.')
    self.assertEqual(orig.AllPayloads(), expected_all_payloads)

    # Exercises implementation details, but is simple and might be useful to
    # narrow down root causes for regressions.
    redx = orig.__reduce_ex__()
    self.assertLen(redx, 2)
    self.assertIs(redx[0], status.Status)
    self.assertEqual(
        redx[1],
        (status.InitFromTag.serialized,
         (status.StatusCode.CANCELLED, b'Cucumber.', expected_all_payloads)))

    ser = pickle.dumps(orig)
    deser = pickle.loads(ser)
    self.assertEqual(deser, orig)
    self.assertIs(deser.__class__, orig.__class__)

  def test_init_from_serialized_exception_unexpected_len_state(self):
    with self.assertRaisesRegex(
        ValueError, r'Unexpected len\(state\) == 4'
        r' \[.*register_status_bindings\.cc:[0-9]+\]'):
      status.Status(status.InitFromTag.serialized, (0, 0, 0, 0))

  def test_init_from_serialized_exception_unexpected_len_ap_item_tup(self):
    with self.assertRaisesRegex(
        ValueError,
        r'Unexpected len\(tuple\) == 3 where \(type_url, payload\) is expected'
        r' \[.*register_status_bindings\.cc:[0-9]+\]'):
      status.Status(status.InitFromTag.serialized,
                    (status.StatusCode.CANCELLED, '', ((0, 0, 0),)))

  def test_init_from_capsule_direct_ok(self):
    orig = status.Status(status.StatusCode.CANCELLED, 'Direct.')
    from_cap = status.Status(status.InitFromTag.capsule, orig.as_absl_Status())
    self.assertEqual(from_cap, orig)

  def test_init_from_capsule_as_capsule_method_ok(self):
    orig = status.Status(status.StatusCode.CANCELLED, 'AsCapsuleMethod.')
    from_cap = status.Status(status.InitFromTag.capsule, orig)
    self.assertEqual(from_cap, orig)

  @parameterized.parameters((False, 'NULL'), (True, '"NotGood"'))
  def test_init_from_capsule_direct_bad_capsule(self, pass_name, quoted_name):
    with self.assertRaises(ValueError) as ctx:
      status.Status(status.InitFromTag.capsule,
                    status_example.make_bad_capsule(pass_name))
    self.assertEqual(
        str(ctx.exception),
        f'obj is a capsule with name {quoted_name} but "::absl::Status"'
        f' is expected.')

  @parameterized.parameters((False, 'NULL'), (True, '"NotGood"'))
  def test_init_from_capsule_correct_method_bad_capsule(self, pass_name,
                                                        quoted_name):
    with self.assertRaises(ValueError) as ctx:
      status.Status(status.InitFromTag.capsule, BadCapsule(pass_name))
    self.assertEqual(
        str(ctx.exception),
        f'BadCapsule.as_absl_Status() returned a capsule with name'
        f' {quoted_name} but "::absl::Status" is expected.')

  @parameterized.parameters(None, '', 0)
  def test_init_from_capsule_direct_only_not_a_capsule(self, not_a_capsule):
    with self.assertRaises(ValueError) as ctx:
      status.Status(status.InitFromTag.capsule_direct_only, not_a_capsule)
    nm = not_a_capsule.__class__.__name__
    self.assertEqual(str(ctx.exception), f'{nm} object is not a capsule.')

  @parameterized.parameters(None, '', 0)
  def test_init_from_capsule_direct_not_a_capsule(self, not_a_capsule):
    with self.assertRaises(ValueError) as ctx:
      status.Status(status.InitFromTag.capsule, not_a_capsule)
    nm = not_a_capsule.__class__.__name__
    self.assertEqual(
        str(ctx.exception),
        f"{nm}.as_absl_Status() call failed: AttributeError: '{nm}' object"
        f" has no attribute 'as_absl_Status'")

  @parameterized.parameters(None, '', 0)
  def test_init_from_capsule_correct_method_not_a_capsule(self, not_a_capsule):
    with self.assertRaises(ValueError) as ctx:
      status.Status(status.InitFromTag.capsule, NotACapsule(not_a_capsule))
    nm = not_a_capsule.__class__.__name__
    self.assertEqual(
        str(ctx.exception),
        f'NotACapsule.as_absl_Status() returned an object ({nm})'
        f' that is not a capsule.')


class IntGetter(status_example.IntGetter):

  def Get(self, i):
    if i > 10:
      raise ValueError('Value out of range')
    return i


class StatusOrTest(absltest.TestCase):

  def test_return_value_status_or_return_type_from_doc(self):
    self.assertEndsWith(
        docstring_signature(status_example.return_value_status_or),
        ' -> int')

  def test_return_value(self):
    self.assertEqual(status_example.return_value_status_or(5), 5)

  def test_return_not_ok(self):
    with self.assertRaises(status.StatusNotOk) as cm:
      status_example.return_failure_status_or(status.StatusCode.NOT_FOUND)
    self.assertEqual(cm.exception.status.code(), status.StatusCode.NOT_FOUND)

  def test_make_failure_status_or_return_type_from_doc(self):
    self.assertRegex(
        docstring_signature(status_example.make_failure_status_or),
        r' -> Union\[.*\.Status, int\]')

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

  def test_return_status_or_pointer(self):
    expected_result = 42
    for _ in range(3):
      result = status_example.return_status_or_pointer()
      self.assertEqual(result, expected_result)

  def test_return_failed_status_or_pointer(self):
    for _ in range(3):
      with self.assertRaises(status.StatusNotOk):
        status_example.return_failure_status_or_pointer()

  def test_canonical_error_to_string(self):
    failure_result = status_example.make_failure_status_or(
        status.StatusCode.CANCELLED)
    self.assertEqual(failure_result.to_string(), 'CANCELLED: ')
    self.assertEqual(str(failure_result), 'CANCELLED: ')

  def test_overriding_in_python(self):
    int_getter = IntGetter()
    self.assertEqual(int_getter.Get(5), 5)
    with self.assertRaises(ValueError):
      int_getter.Get(100)
    self.assertEqual(
        status_example.call_get_redirect_to_python(int_getter, 5), 5)
    with self.assertRaises(ValueError):
      status_example.call_get_redirect_to_python(int_getter, 100)

if __name__ == '__main__':
  absltest.main()
