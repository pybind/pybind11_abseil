import pickle

from absl.testing import absltest
from absl.testing import parameterized
from pybind11_abseil import status


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

  def test_repr(self):
    self.assertEqual(
        repr(status.StatusCode.NOT_FOUND), '<StatusCode.NOT_FOUND: 5>')


class StatusTest(parameterized.TestCase):

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

  def test_canonical_error(self):
    test_status = status.aborted_error('test')
    self.assertEqual(test_status.code(), status.StatusCode.ABORTED)
    self.assertEqual(test_status.message(), 'test')

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
    self.assertEqual(str(e), '� [INVALID_ARGUMENT]')

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

  def test_eq_and_hash(self):
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


class StatusNotOkTest(absltest.TestCase):

  def test_build_status_not_ok_enum(self):
    e = status.BuildStatusNotOk(status.StatusCode.INVALID_ARGUMENT, 'Msg enum.')
    self.assertEqual(e.status.code(), status.StatusCode.INVALID_ARGUMENT)
    self.assertEqual(e.code, int(status.StatusCode.INVALID_ARGUMENT))
    self.assertEqual(e.message, 'Msg enum.')

  def test_build_status_not_ok_int(self):
    with self.assertRaises(TypeError) as cm:
      status.BuildStatusNotOk(1, 'Msg int.')  # pytype: disable=wrong-arg-types
    self.assertIn('incompatible function arguments', str(cm.exception))

  def test_eq(self):
    sa1 = status.BuildStatusNotOk(status.StatusCode.UNKNOWN, 'sa')
    sa2 = status.BuildStatusNotOk(status.StatusCode.UNKNOWN, 'sa')
    sb = status.BuildStatusNotOk(status.StatusCode.UNKNOWN, 'sb')
    self.assertTrue(bool(sa1 == sa1))  # pylint: disable=comparison-with-itself
    self.assertTrue(bool(sa1 == sa2))
    self.assertFalse(bool(sa1 == sb))
    self.assertFalse(bool(sa1 == 'x'))
    self.assertFalse(bool('x' == sa1))

  def test_pickle(self):
    orig = status.BuildStatusNotOk(status.StatusCode.UNKNOWN, 'Cabbage')
    ser = pickle.dumps(orig)
    deser = pickle.loads(ser)
    self.assertEqual(deser.message, 'Cabbage')
    self.assertEqual(deser, orig)
    self.assertIs(deser.__class__, orig.__class__)


if __name__ == '__main__':
  absltest.main()
