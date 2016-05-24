import unittest

import ModelHighAPI

class DoubleTestCase(unittest.TestCase):

    def test_create_default(self):
        default = ModelHighAPI.ModelHighAPI_Double()

    def test_create_from_double(self):
        from_double = ModelHighAPI.ModelHighAPI_Double(100.)
        self.assertEqual(100., from_double.value())

    def test_create_from_text(self):
        from_string = ModelHighAPI.ModelHighAPI_Double("200 + x")
        self.assertEqual("200 + x", from_string.text())
    

if __name__ == "__main__":
    unittest.main()
