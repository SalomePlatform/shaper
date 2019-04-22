# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

import salome, os, sys
import SalomePyQt

import unittest
import subprocess

import ModelHighAPI, ModelAPI, PartSetAPI
from GeomAPI import GeomAPI_Shape
from salome.shaper import model

class TestHDF(unittest.TestCase):
  testfile = ""
  reffile = ""

  def setUp(self):
    salome.salome_close()

    # leave file name only (trim path and extension)
    fileName = os.path.basename(self.testfile)
    self.reffile = self.reffile + "/" + os.path.splitext(fileName)[0] + ".py"

    salome.salome_init(self.testfile, embedded=1)
    myStudyName = salome.myStudy._get_Name()
    self.session = salome.naming_service.Resolve('/Kernel/Session')
    self.session.emitMessage("connect_to_study")

    self.sg = SalomePyQt.SalomePyQt()
    self.sg.activateModule("Shaper")
    self.session = ModelAPI.ModelAPI_Session.get()
    self.partSet = self.session.moduleDocument()

  def tearDown(self):
    salome.sg.UpdateView()
    self.sg.processEvents()
    salome.sg.FitAll()

  def test_hdf_file(self):
    self.assertTrue(self.partSet.size("Parts") > 0)
    aPartsList = []
    for aPartIndex in range(self.partSet.size("Parts")):
      self.session.startOperation()
      aPart = ModelAPI.modelAPI_ResultPart(ModelAPI.objectToResult(self.partSet.object("Parts", aPartIndex)))
      aPart.activate()
      self.session.finishOperation()

      aPartFeature = PartSetAPI.PartSetAPI_Part(self.partSet.currentFeature(True))
      # check reference data
      exec(open(self.reffile, "rb").read())


if __name__ == "__main__":
  if len(sys.argv) > 1:
    TestHDF.testfile = sys.argv[1]
  if len(sys.argv) > 2:
    salomePortFile = sys.argv[2]
  if len(sys.argv) > 3:
    errFile = open(sys.argv[3], 'w')
  if len(sys.argv) > 4:
    salomeKernelDir = sys.argv[4]
  if len(sys.argv) > 5:
    TestHDF.reffile = sys.argv[5]

  aTest = unittest.TestLoader().loadTestsFromTestCase(TestHDF)
  unittest.TextTestRunner(stream=errFile).run(aTest)
  errFile.close()

  # close Salome GUI
  proc = subprocess.Popen(salomeKernelDir + "/bin/salome/killSalome.py")

  try:
    os.remove(salomePortFile)
  except:
    print("Cannot remove file", file=f)
