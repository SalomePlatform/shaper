import unittest

import ModelHighAPI

class DoubleTestCase(unittest.TestCase):

    def test_create_default(self):
        default = ModelHighAPI.ModelHighAPI_Double()

    def test_create_from_double(self):
        from_double = ModelHighAPI.ModelHighAPI_Double(100.)

    def test_create_from_text(self):
        from_string = ModelHighAPI.ModelHighAPI_Double("200 + x")


if __name__ == "__main__":
    unittest.main()
