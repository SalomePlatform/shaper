import unittest

import ModelAPI
import ExchangeAPI
import model

class ExchangeTestCase(unittest.TestCase):

    def setUp(self):
        self.session = ModelAPI.ModelAPI_Session.get()
        self.doc = self.session.moduleDocument()

    def tearDown(self):
        # assert(model.checkPythonDump())
        # This test just checks interface of Exchange High API
        # Export feature stays in the tree because it's invalid, but after dump it is not in the tree.
        self.session.closeAll()

    def test_addImport(self):
        self.session.startOperation()
        self.feature = ExchangeAPI.addImport(self.doc, "file_path")
        self.session.finishOperation()

    def test_addExport(self):
        self.session.startOperation()
        self.feature = ExchangeAPI.exportToFile(self.doc, "file_path", [], "file_format")
        self.session.finishOperation()

if __name__ == "__main__":
    unittest.main()
