# Copyright (c) 2019-2022 The Pybind Development Team. All rights reserved.
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.

import sys

from absl.testing import absltest

from pybind11_abseil import ok_status_singleton


class OkStatusSingletonTest(absltest.TestCase):

  def test_singleton(self):
    cap = ok_status_singleton.OkStatusSingleton()
    if 'pybind11_abseil.status' in sys.modules:  # OSS
      expected_repr_cap = '<pybind11_abseil.status.Status object at 0x'
    else:
      expected_repr_cap = '<capsule object "::absl::Status" at 0x'
    self.assertStartsWith(repr(cap), expected_repr_cap)

if __name__ == '__main__':
  absltest.main()
