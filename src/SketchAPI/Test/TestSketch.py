import unittest

import ModelAPI
import ExchangeAPI

class PointTestCase(unittest.TestCase):

    def setUp(self):
        self.session = ModelAPI.ModelAPI_Session.get()
        self.doc = self.session.moduleDocument()

    def tearDown(self):
        self.session.closeAll()

    def test_addImport(self):
        self.session.startOperation()
        self.feature = ExchangeAPI.addImport(self.doc, "file_path")
        self.session.finishOperation()

    def test_addExport(self):
        self.session.startOperation()
        self.feature = ExchangeAPI.exportToFile(self.doc, "file_path", "file_format", [])
        self.session.finishOperation()

if __name__ == "__main__":
    unittest.main()
