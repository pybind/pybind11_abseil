"""Tests that the casters raise a TypeError if the status module is missing."""

# In google3, `from absl.testing import absltest` imports the status module,
# which breaks the requirement for this test that the status module is missing.
import unittest

from pybind11_abseil.tests import missing_import


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
