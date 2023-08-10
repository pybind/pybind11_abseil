# Copyright (c) 2019-2022 The Pybind Development Team. All rights reserved.
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.

from absl.testing import absltest

from pybind11_abseil import ok_status_singleton


class OkStatusSingletonTest(absltest.TestCase):

  def test_singleton(self):
    cap = ok_status_singleton.OkStatusSingleton()
    cap_repr = repr(cap)

    # Modified to reflect the alternate behavior with CMake build
    self.assertTrue(cap_repr.startswith(
      '<capsule object "::absl::Status" at 0x') or cap_repr.startswith( 
      '<pybind11_abseil.status.Status object at 0x'))


if __name__ == '__main__':
  absltest.main()
