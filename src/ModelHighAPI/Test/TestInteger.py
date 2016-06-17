import unittest

import ModelHighAPI

class IntegerTestCase(unittest.TestCase):

    def test_create_default(self):
        default = ModelHighAPI.ModelHighAPI_Integer()

    def test_create_from_integer(self):
        from_integer = ModelHighAPI.ModelHighAPI_Integer(100)

    def test_create_from_text(self):
        from_string = ModelHighAPI.ModelHighAPI_Integer("200 + x")


if __name__ == "__main__":
    unittest.main()
