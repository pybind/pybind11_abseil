"""Tests that the casters raise a TypeError if the status module is missing."""

from pybind11_abseil.tests import missing_import
import unittest


class MissingStatusImportTest(unittest.TestCase):

  message_regex = 'Status module has not been imported.*'

  def test_returns_status(self):
    with self.assertRaisesRegex(TypeError, self.message_regex):
      missing_import.returns_status()

  def test_returns_status_or(self):
    with self.assertRaisesRegex(TypeError, self.message_regex):
      missing_import.returns_status_or()


if __name__ == '__main__':
  unittest.main()
