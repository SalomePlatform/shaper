import unittest

class Test(unittest.TestCase):
    def test_assert_false(self):
        self.assertFalse(False)

if __name__ == "__main__":
    unittest.main()
