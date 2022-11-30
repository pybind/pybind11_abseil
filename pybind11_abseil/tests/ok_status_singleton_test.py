# Copyright (c) 2019-2022 The Pybind Development Team. All rights reserved.
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.

from absl.testing import absltest

from pybind11_abseil import ok_status_singleton


class OkStatusSingletonTest(absltest.TestCase):

  def test_singleton(self):
    cap = ok_status_singleton.OkStatusSingleton()
    self.assertStartsWith(repr(cap), '<capsule object "::absl::Status" at 0x')


if __name__ == '__main__':
  absltest.main()
