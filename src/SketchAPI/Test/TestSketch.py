import unittest

import ModelAPI
import SketchAPI

class SketchTestCase(unittest.TestCase):

    def setUp(self):
        self.session = ModelAPI.ModelAPI_Session.get()
        self.doc = self.session.moduleDocument()

    def tearDown(self):
        self.session.closeAll()

# TODO(spo): add tests.

if __name__ == "__main__":
    unittest.main()
