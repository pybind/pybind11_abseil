# Copyright (c) 2019-2021 The Pybind Development Team. All rights reserved.
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.
"""Tests for absl pybind11 casters."""

import array
import datetime

from absl.testing import absltest
from absl.testing import parameterized
from dateutil import tz
import numpy as np

from pybind11_abseil.tests import absl_example


class AbslTimeTest(parameterized.TestCase):
  SECONDS_IN_DAY = 24 * 60 * 60
  POSITIVE_SECS = 3 * SECONDS_IN_DAY + 2.5
  NEGATIVE_SECS = -3 * SECONDS_IN_DAY + 2.5
  TEST_DATETIME = datetime.datetime(2000, 1, 2, 3, 4, 5, int(5e5))
  # Linter error relevant for pytz only.
  # pylint: disable=g-tzinfo-replace
  TEST_DATETIME_UTC = TEST_DATETIME.replace(tzinfo=tz.tzutc())
  # pylint: enable=g-tzinfo-replace
  TEST_DATE = datetime.date(2000, 1, 2)

  def test_return_positive_duration(self):
    duration = absl_example.make_duration(self.POSITIVE_SECS)
    self.assertEqual(duration.days, 3)
    self.assertEqual(duration.seconds, 2)
    self.assertEqual(duration.microseconds, 5e5)

  def test_return_negative_duration(self):
    duration = absl_example.make_duration(self.NEGATIVE_SECS)
    self.assertEqual(duration.days, -3)
    self.assertEqual(duration.seconds, 2)
    self.assertEqual(duration.microseconds, 5e5)

  def test_pass_positive_duration(self):
    duration = datetime.timedelta(seconds=self.POSITIVE_SECS)
    self.assertTrue(absl_example.check_duration(duration, self.POSITIVE_SECS))

  def test_pass_negative_duration(self):
    duration = datetime.timedelta(seconds=self.NEGATIVE_SECS)
    self.assertTrue(absl_example.check_duration(duration, self.NEGATIVE_SECS))

  def test_return_datetime(self):
    secs = self.TEST_DATETIME.timestamp()
    local_tz = tz.gettz()
    # pylint: disable=g-tzinfo-datetime
    # Warning about tzinfo applies to pytz, but we are using dateutil.tz
    expected_datetime = datetime.datetime(
        year=self.TEST_DATETIME.year,
        month=self.TEST_DATETIME.month,
        day=self.TEST_DATETIME.day,
        hour=self.TEST_DATETIME.hour,
        minute=self.TEST_DATETIME.minute,
        second=self.TEST_DATETIME.second,
        microsecond=self.TEST_DATETIME.microsecond,
        tzinfo=local_tz)
    # pylint: enable=g-tzinfo-datetime
    # datetime handling code will set the local timezone on C++ times for want
    # of a better alternative
    self.assertEqual(expected_datetime, absl_example.make_datetime(secs))

  def test_pass_date(self):
    secs = datetime.datetime(self.TEST_DATE.year, self.TEST_DATE.month,
                             self.TEST_DATE.day).timestamp()
    self.assertTrue(absl_example.check_datetime(self.TEST_DATE, secs))

  def test_pass_datetime(self):
    secs = self.TEST_DATETIME.timestamp()
    self.assertTrue(absl_example.check_datetime(self.TEST_DATETIME, secs))

  def test_pass_datetime_with_timezone(self):
    pacific_tz = tz.gettz('America/Los_Angeles')
    # pylint: disable=g-tzinfo-datetime
    # Warning about tzinfo applies to pytz, but we are using dateutil.tz
    dt_with_tz = datetime.datetime(
        year=2020, month=2, day=1, hour=20, tzinfo=pacific_tz)
    # pylint: enable=g-tzinfo-datetime
    secs = dt_with_tz.timestamp()
    self.assertTrue(absl_example.check_datetime(dt_with_tz, secs))

  def test_pass_datetime_dst_with_timezone(self):
    pacific_tz = tz.gettz('America/Los_Angeles')
    # pylint: disable=g-tzinfo-datetime
    dst_end = datetime.datetime(2020, 11, 1, 2, 0, 0, tzinfo=pacific_tz)
    # pylint: enable=g-tzinfo-datetime
    secs = dst_end.timestamp()
    self.assertTrue(absl_example.check_datetime(dst_end, secs))

  def test_pass_datetime_dst(self):
    dst_end = datetime.datetime(2020, 11, 1, 2, 0, 0)
    secs = dst_end.timestamp()
    self.assertTrue(absl_example.check_datetime(dst_end, secs))

  @parameterized.named_parameters(('before', -1), ('flip', 0), ('after', 1))
  def test_dst_datetime_from_timestamp(self, offs):
    secs_flip = 1604224799  # 2020-11-01T02:00:00-08:00
    secs = secs_flip + offs
    time_utc = datetime.datetime.fromtimestamp(secs, tz.tzutc())
    time_local_aware = time_utc.astimezone(tz.gettz())
    time_local_naive = time_local_aware.replace(tzinfo=None)
    for time in (time_utc, time_local_aware, time_local_naive):
      self.assertTrue(absl_example.check_datetime(time, secs))

  def test_pass_datetime_pre_unix_epoch(self):
    dt = datetime.datetime(1969, 7, 16, 10, 56, 7, microsecond=140)
    secs = dt.timestamp()
    self.assertTrue(absl_example.check_datetime(dt, secs))

  def test_return_civilsecond(self):
    # We need to use a timezone aware datetime here, otherwise
    # datetime.timestamp converts to localtime. UTC is chosen as the convention
    # in the test cases.
    truncated = self.TEST_DATETIME.replace(microsecond=0)
    self.assertEqual(
        truncated,
        absl_example.make_civilsecond(self.TEST_DATETIME_UTC.timestamp()))

  def test_pass_datetime_as_civilsecond(self):
    truncated = self.TEST_DATETIME_UTC.replace(microsecond=0)
    self.assertTrue(
        absl_example.check_civilsecond(self.TEST_DATETIME,
                                       truncated.timestamp()))

  def test_return_civilminute(self):
    truncated = self.TEST_DATETIME.replace(second=0, microsecond=0)
    self.assertEqual(
        truncated,
        absl_example.make_civilminute(self.TEST_DATETIME_UTC.timestamp()))

  def test_pass_datetime_as_civilminute(self):
    truncated = self.TEST_DATETIME_UTC.replace(second=0, microsecond=0)
    self.assertTrue(
        absl_example.check_civilminute(self.TEST_DATETIME,
                                       truncated.timestamp()))

  def test_return_civilhour(self):
    truncated = self.TEST_DATETIME.replace(minute=0, second=0, microsecond=0)
    self.assertEqual(
        truncated,
        absl_example.make_civilhour(self.TEST_DATETIME_UTC.timestamp()))

  def test_pass_datetime_as_civilhour(self):
    truncated = self.TEST_DATETIME_UTC.replace(
        minute=0, second=0, microsecond=0)
    self.assertTrue(
        absl_example.check_civilhour(self.TEST_DATETIME, truncated.timestamp()))

  def test_return_civilday(self):
    truncated = self.TEST_DATETIME.replace(
        hour=0, minute=0, second=0, microsecond=0)
    self.assertEqual(
        truncated,
        absl_example.make_civilday(self.TEST_DATETIME_UTC.timestamp()))

  def test_pass_datetime_as_civilday(self):
    truncated = self.TEST_DATETIME_UTC.replace(
        hour=0, minute=0, second=0, microsecond=0)
    self.assertTrue(
        absl_example.check_civilday(self.TEST_DATETIME, truncated.timestamp()))

  def test_return_civilmonth(self):
    truncated = self.TEST_DATETIME.replace(
        day=1, hour=0, minute=0, second=0, microsecond=0)
    self.assertEqual(
        truncated,
        absl_example.make_civilmonth(self.TEST_DATETIME_UTC.timestamp()))

  def test_pass_datetime_as_civilmonth(self):
    truncated = self.TEST_DATETIME_UTC.replace(
        day=1, hour=0, minute=0, second=0, microsecond=0)
    self.assertTrue(
        absl_example.check_civilmonth(self.TEST_DATETIME,
                                      truncated.timestamp()))

  def test_return_civilyear(self):
    truncated = self.TEST_DATETIME.replace(
        month=1, day=1, hour=0, minute=0, second=0, microsecond=0)
    self.assertEqual(
        truncated,
        absl_example.make_civilyear(self.TEST_DATETIME_UTC.timestamp()))

  def test_pass_datetime_as_civilyear(self):
    truncated = self.TEST_DATETIME_UTC.replace(
        month=1, day=1, hour=0, minute=0, second=0, microsecond=0)
    self.assertTrue(
        absl_example.check_civilyear(self.TEST_DATETIME, truncated.timestamp()))


def make_read_only_numpy_array():
  values = np.zeros(5, dtype=np.int32)
  values.flags.writeable = False
  return values


def make_srided_numpy_array(stride):
  return np.zeros(10, dtype=np.int32)[::stride]


class AbslNumericSpanTest(parameterized.TestCase):

  # The check_span* functions use signed ints. Arrays with different numeric
  # types and all non-array types (eg native sequence types) require converting.
  CONVERTED_NUMERIC_LISTS = (
      ('array_wrong_dtype', array.array('b', [9, 8, 7])),
      ('numpy_wrong_dtype', np.array([7, 8, 9], dtype=np.uint16)),
      ('tuple', (1, 2, 3)),
      ('list', [4, 5, 6]),
  )

  NOT_CONVERTED_NUMERIC_LISTS = (
      ('array_matching_dtype', array.array('i', [9, 8, 7])),
      ('numpy_matching_dtype', np.array([7, 8, 9], dtype=np.int32)),
  )

  NUMERIC_LISTS = CONVERTED_NUMERIC_LISTS + NOT_CONVERTED_NUMERIC_LISTS

  def test_return_span(self):
    values = [1, 2, 3, 4]
    container = absl_example.VectorContainer()
    self.assertSequenceEqual(container.make_span(values), values)

  @parameterized.named_parameters(*NUMERIC_LISTS)
  def test_pass_span_from(self, values):
    # Pass values twice- one will be converted to a span, the other to a vector
    # (which is known to work), and then they will be compared.
    self.assertTrue(absl_example.check_span(values, values))

  @parameterized.named_parameters(*NUMERIC_LISTS)
  def test_caster_copy_from(self, values):
    self.assertTrue(absl_example.check_span_caster_copy(values, values))

  @parameterized.named_parameters(*NOT_CONVERTED_NUMERIC_LISTS)
  def test_pass_span_no_convert_from(self, values):
    self.assertTrue(absl_example.check_span_no_convert(values, values))

  @parameterized.named_parameters(*CONVERTED_NUMERIC_LISTS)
  def test_pass_span_no_convert_fails_from(self, values):
    with self.assertRaises(TypeError):
      absl_example.check_span_no_convert(values, values)

  @parameterized.named_parameters(
      ('string', 'test'), ('int', 5), ('object', absl_example.ObjectForSpan(3)),
      ('list_of_objects', [absl_example.ObjectForSpan(3)]))
  def test_pass_span_fails_from(self, values):
    with self.assertRaises(TypeError):
      absl_example.check_span(values, values)

  def test_fill_span_from_numpy(self):
    values = np.zeros(5, dtype=np.int32)
    absl_example.fill_span(42, values)
    for e in values:
      self.assertEqual(e, 42)

  @parameterized.named_parameters(
      ('float_numpy', np.zeros(5, dtype=np.float)),
      ('two_d_numpy', np.zeros(
          (5, 5), dtype=np.int32)), ('read_only', make_read_only_numpy_array()),
      ('strided_skip', make_srided_numpy_array(2)),
      ('strided_reverse', make_srided_numpy_array(-1)),
      ('non_supported_type', np.zeros(5, dtype=np.unicode_)),
      ('native_list', [0] * 5))
  def test_fill_span_fails_from(self, values):
    with self.assertRaises(TypeError):
      absl_example.fill_span(42, values)


def make_native_list_of_objects():
  return [absl_example.ObjectForSpan(3), absl_example.ObjectForSpan(5)]


def make_opaque_list_of_objects():
  return absl_example.ObjectVector(make_native_list_of_objects())


def make_list_of_objects_parameters():
  return (('native_list', make_native_list_of_objects()),
          ('opaque_list', make_opaque_list_of_objects()))


class AbslObjectSpanTest(parameterized.TestCase):

  @parameterized.named_parameters(*make_list_of_objects_parameters())
  def test_pass_object_pointers_span_from(self, objects):
    self.assertEqual(absl_example.sum_object_pointers_span(objects), 8)

  @parameterized.named_parameters(*make_list_of_objects_parameters())
  def test_pass_object_span_from(self, objects):
    self.assertEqual(absl_example.sum_object_span(objects), 8)

  def test_pass_object_span_no_convert_from_opaque_list(self):
    absl_example.sum_object_span_no_convert(make_opaque_list_of_objects())

  def test_pass_object_span_no_convert_fails_from_native_list(self):
    # Native list requires conversion.
    with self.assertRaises(TypeError):
      absl_example.sum_object_span_no_convert(make_native_list_of_objects())

  @parameterized.named_parameters(*make_list_of_objects_parameters())
  def test_fill_object_pointers_span_from(self, objects):
    absl_example.fill_object_pointers_span(42, objects)
    for obj in objects:
      self.assertEqual(obj.value, 42)

  def test_fill_object_span_no_convert_from_opaque_list(self):
    objects = make_opaque_list_of_objects()
    absl_example.fill_object_span(42, objects)
    for obj in objects:
      self.assertEqual(obj.value, 42)

  def test_fill_object_span_no_convert_fails_from_native_list(self):
    objects = make_native_list_of_objects()
    # Native list requires conversion.
    with self.assertRaises(TypeError):
      absl_example.fill_object_span(objects)


class AbslStringViewTest(absltest.TestCase):
  TEST_STRING = 'test string!'

  def test_return_view(self):
    container = absl_example.StringContainer()
    self.assertSequenceEqual(
        container.make_string_view(self.TEST_STRING), self.TEST_STRING)

  def test_pass_string_view(self):
    self.assertTrue(
        absl_example.check_string_view(self.TEST_STRING, self.TEST_STRING))


class AbslCordTest(absltest.TestCase):
  TEST_STRING = 'absl_Cord'
  TEST_BYTES = b'absl_Cord'

  def test_return_absl_cord(self):
    self.assertSequenceEqual(
        absl_example.return_absl_cord(self.TEST_STRING), self.TEST_STRING)
    self.assertSequenceEqual(
        absl_example.return_absl_cord(self.TEST_BYTES), self.TEST_STRING)

  def test_pass_absl_cord(self):
    self.assertTrue(
        absl_example.check_absl_cord(self.TEST_STRING, self.TEST_STRING))
    self.assertFalse(
        absl_example.check_absl_cord(self.TEST_STRING, '12345'))
    self.assertTrue(
        absl_example.check_absl_cord(self.TEST_BYTES, self.TEST_STRING))


class AbslFlatHashMapTest(absltest.TestCase):

  def test_return_map(self):
    keys_and_values = [(1, 2), (3, 4), (5, 6)]
    expected = dict(keys_and_values)
    self.assertEqual(expected, absl_example.make_map(keys_and_values))

  def test_pass_map(self):
    expected = [(10, 20), (30, 40)]
    self.assertTrue(absl_example.check_map(dict(expected), expected))


class AbslFlatHashSetTest(absltest.TestCase):

  def test_return_set(self):
    values = [1, 3, 7, 5]
    expected = set(values)
    self.assertEqual(expected, absl_example.make_set(values))

  def test_pass_set(self):
    expected = [10, 20, 30, 40]
    self.assertTrue(absl_example.check_set(set(expected), expected))


class AbslBTreeMapTest(absltest.TestCase):

  def test_return_map(self):
    keys_and_values = [(1, 2), (3, 4), (5, 6)]
    expected = dict(keys_and_values)
    self.assertEqual(expected, absl_example.make_btree_map(keys_and_values))

  def test_pass_map(self):
    expected = [(10, 20), (30, 40)]
    self.assertTrue(absl_example.check_btree_map(dict(expected), expected))


class AbslOptionalTest(absltest.TestCase):

  def test_pass_default_nullopt(self):
    self.assertTrue(absl_example.check_optional())

  def test_pass_value(self):
    self.assertTrue(absl_example.check_optional(5, True, 5))

  def test_pass_none(self):
    self.assertTrue(absl_example.check_optional(None, False))

  def test_return_value(self):
    self.assertEqual(absl_example.make_optional(5), 5)

  def test_return_none(self):
    self.assertIsNone(absl_example.make_optional())


class AbslVariantTest(absltest.TestCase):

  def test_variant(self):
    assert absl_example.VariantToInt(absl_example.A(3)) == 3
    assert absl_example.VariantToInt(absl_example.B(5)) == 5

    for identity_f, should_be_equal in [(absl_example.Identity, True),
                                        (absl_example.IdentityWithCopy, False)]:
      objs = [absl_example.A(3), absl_example.B(5)]
      vector = identity_f(objs)
      self.assertLen(vector, 2)
      self.assertIsInstance(vector[0], absl_example.A)
      self.assertEqual(vector[0].a, 3)
      self.assertIsInstance(vector[1], absl_example.B)
      self.assertEqual(vector[1].b, 5)
      if should_be_equal:
        self.assertEqual(objs, vector)
      else:
        self.assertNotEqual(objs, vector)


if __name__ == '__main__':
  absltest.main()
