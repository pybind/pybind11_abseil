# Copyright (c) 2019 The Pybind Development Team. All rights reserved.
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.
"""Tests for absl pybind11 casters."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import datetime
from dateutil import tz
import numpy as np

from google3.testing.pybase import googletest
from google3.testing.pybase import parameterized
from pybind11_abseil import absl_example


class AbslTimeTest(googletest.TestCase):
  SECONDS_IN_DAY = 24 * 60 * 60
  POSITIVE_SECS = 3 * SECONDS_IN_DAY + 2.5
  NEGATIVE_SECS = -3 * SECONDS_IN_DAY + 2.5
  TEST_DATETIME = datetime.datetime(2000, 1, 2, 3, 4, 5, int(5e5))
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


class AbslSpanTest(parameterized.TestCase):

  def test_return_span(self):
    values = [1, 2, 3, 4]
    container = absl_example.VectorContainer()
    self.assertSequenceEqual(container.make_span(values), values)

  @parameterized.named_parameters(('list', [2, 4, 6]), ('tuple', (1, 3, 5, 7)),
                                  ('numpy', np.array([7, 8, 9])))
  def test_pass_span_from(self, values):
    # Pass values twice- one will be converted to a span, the other to a vector
    # (which is known to work), and then they will be compared.
    self.assertTrue(absl_example.check_span(values, values))


class AbslStringViewTest(googletest.TestCase):
  TEST_STRING = 'test string!'

  def test_return_view(self):
    container = absl_example.StringContainer()
    self.assertSequenceEqual(
        container.make_string_view(self.TEST_STRING), self.TEST_STRING)

  def test_pass_string_view(self):
    self.assertTrue(
        absl_example.check_string_view(self.TEST_STRING, self.TEST_STRING))


class AbslFlatHashMapTest(googletest.TestCase):

  def test_return_map(self):
    keys_and_values = [(1, 2), (3, 4), (5, 6)]
    expected = dict(keys_and_values)
    self.assertEqual(expected, absl_example.make_map(keys_and_values))

  def test_pass_map(self):
    expected = [(10, 20), (30, 40)]
    self.assertTrue(absl_example.check_map(dict(expected), expected))


class AbslOptionalTest(googletest.TestCase):

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


if __name__ == '__main__':
  googletest.main()
