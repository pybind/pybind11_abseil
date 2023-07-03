from absl.testing import absltest
from absl.testing import parameterized

from pybind11_abseil.tests import cpp_capsule_tools_testing as tstng


class UsingMakeCapsule:

  def __init__(self, make_capsule):
    self.make_capsule = make_capsule

  def get_capsule(self):
    return self.make_capsule()


class BadCapsule:

  def __init__(self, pass_name):
    self.pass_name = pass_name

  def get_capsule(self):
    return tstng.make_bad_capsule(self.pass_name)


class NotACapsule:

  def __init__(self, not_a_capsule):
    self.not_a_capsule = not_a_capsule

  def get_capsule(self):
    return self.not_a_capsule


class RaisingGetCapsule:

  def get_capsule(self):
    raise RuntimeError('from get_capsule')


class CppCapsuleToolsTest(parameterized.TestCase):

  def test_raw_ptr_capsule_direct(self):
    cap = tstng.make_raw_ptr_capsule()
    res = tstng.get_int_from_raw_ptr_capsule(cap, False)
    self.assertEqual(res, '890352')

  def test_raw_ptr_capsule_method(self):
    using_cap = UsingMakeCapsule(tstng.make_raw_ptr_capsule)
    res = tstng.get_int_from_raw_ptr_capsule(using_cap, True)
    self.assertEqual(res, '890352')

  def test_shared_ptr_capsule_direct(self):
    cap = tstng.make_shared_ptr_capsule()
    res = tstng.get_int_from_shared_ptr_capsule(cap, False)
    self.assertEqual(res, '906069')

  def test_shared_ptr_capsule_method(self):
    using_cap = UsingMakeCapsule(tstng.make_shared_ptr_capsule)
    res = tstng.get_int_from_shared_ptr_capsule(using_cap, True)
    self.assertEqual(res, '906069')

  @parameterized.parameters((False, 'NULL'), (True, '"NotGood"'))
  def test_raw_ptr_capsule_direct_bad_capsule(self, pass_name, quoted_name):
    cap = tstng.make_bad_capsule(pass_name)
    res = tstng.get_int_from_raw_ptr_capsule(cap, False)
    self.assertEqual(
        res,
        f'INVALID_ARGUMENT: obj is a capsule with name {quoted_name} but'
        ' "type:int" is expected.',
    )

  @parameterized.parameters((False, 'NULL'), (True, '"NotGood"'))
  def test_raw_ptr_capsule_method_bad_capsule(self, pass_name, quoted_name):
    cap = BadCapsule(pass_name)
    res = tstng.get_int_from_raw_ptr_capsule(cap, True)
    self.assertEqual(
        res,
        'INVALID_ARGUMENT: BadCapsule.get_capsule() returned a capsule with'
        f' name {quoted_name} but "type:int" is expected.',
    )

  @parameterized.parameters(None, '', 0)
  def test_raw_ptr_capsule_direct_not_a_capsule(self, not_a_capsule):
    res = tstng.get_int_from_raw_ptr_capsule(not_a_capsule, False)
    self.assertEqual(
        res,
        f'INVALID_ARGUMENT: {not_a_capsule.__class__.__name__} object is not a'
        ' capsule.',
    )

  @parameterized.parameters(None, '', 0)
  def test_raw_ptr_capsule_method_not_a_capsule(self, not_a_capsule):
    cap = NotACapsule(not_a_capsule)
    res = tstng.get_int_from_raw_ptr_capsule(cap, True)
    self.assertEqual(
        res,
        'INVALID_ARGUMENT: NotACapsule.get_capsule() returned an object'
        f' ({not_a_capsule.__class__.__name__}) that is not a capsule.',
    )

  @parameterized.parameters(
      tstng.get_int_from_raw_ptr_capsule, tstng.get_int_from_shared_ptr_capsule
  )
  def test_raising_get_capsule(self, get_int_from_capsule):
    cap = RaisingGetCapsule()
    res = get_int_from_capsule(cap, True)
    self.assertEqual(
        res,
        'INVALID_ARGUMENT: RaisingGetCapsule.get_capsule() call failed:'
        ' RuntimeError: from get_capsule',
    )


if __name__ == '__main__':
  absltest.main()
