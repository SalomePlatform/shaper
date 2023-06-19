# Copyright (C) 2014-2023  CEA, EDF
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import unittest

import ModelAPI
import ExchangeAPI
from salome.shaper import model

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
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
