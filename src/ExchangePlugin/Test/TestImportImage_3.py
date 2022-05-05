# Copyright (C) 2021-2022  CEA/DEN, EDF R&D
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

import os, inspect
from salome.shaper import model

from PyQt5.Qt import QApplication

import salome
salome.salome_init_without_session()
salome.salome_init(1)
if QApplication.instance() is None:
  app = QApplication([])

from tempfile import TemporaryDirectory
from ModelAPI import *

data_dir = os.path.join(os.path.dirname(inspect.getfile(lambda: None)), "data")

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create ImportImage
ImportImage_1 = model.addImportImage(Part_1_doc, os.path.join(data_dir, "1.jpg"))
model.do()
ImportImage_1.setName("drawing")
ImportImage_1.result().setName("drawing")

### Create Translation
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("FACE", "drawing")], startPoint = model.selection("VERTEX", "[drawing/Shape_1]e[drawing/Shape_4]e"), endPoint = model.selection("VERTEX", "PartSet/Origin"), keepSubResults = True)
Translation_1.setName("translation")

### Create Scale
Scale_1 = model.addScale(Part_1_doc, [model.selection("FACE", "drawing")] , model.selection("VERTEX", "PartSet/Origin"), 0.5, keepSubResults = True)
Scale_1.setName("scale")

model.end()

#=============================================================================
# Tests :
#=============================================================================
model.checkResult(Scale_1, model, 1, [0], [0], [1], [4], [8])
r=Scale_1.defaultResult()
s=r.shape()
dim=s.computeSize()
dim=dim[1:]
dx=abs(dim[3]-dim[0])
dy=abs(dim[4]-dim[1])
dz=abs(dim[5]-dim[2])
tol=1e-06
assert(abs(dx-400) <= tol)
assert(abs(dy-258.5) <= tol)
assert(abs(dz-0) <= tol)
assert(ImportImage_1.result().resultSubShapePair()[0].hasTexture())
assert(Translation_1.result().resultSubShapePair()[0].hasTexture())
assert(Scale_1.result().resultSubShapePair()[0].hasTexture())

aDr = objectToFeature(Part_1_doc.objectByName("Features", "drawing"))
assert(aDr.firstResult().hasTexture())

# check save/load document with an image
with TemporaryDirectory() as tmp_dir:
  aSession = ModelAPI_Session.get()
  aFiles = StringList()
  aSession.save(tmp_dir, aFiles)
  aSession.closeAll()
  assert(aSession.load(tmp_dir))
  model.begin()
  partSet = model.moduleDocument()
  assert(partSet.size("Features") == 1)
  aPart = objectToFeature(partSet.object("Features", 0))
  aPartResult = modelAPI_ResultPart(aPart.results()[0])
  aPartResult.activate()
  aPartDoc = aPartResult.partDoc()
  aSession.setActiveDocument(aPartDoc, True)
  model.end()
  aDr = objectToFeature(aPartDoc.objectByName("Features", "drawing"))
  aTr = objectToFeature(aPartDoc.objectByName("Features", "translation"))
  aSc = objectToFeature(aPartDoc.objectByName("Features", "scale"))
  assert(aDr.firstResult().hasTexture())
  assert(aTr.firstResult().hasTexture())
  assert(aSc.firstResult().hasTexture())

# Close SALOME GUI
import salome_utils
import subprocess
port = salome_utils.getPortNumber()
proc = subprocess.Popen(["killSalomeWithPort.py", "{}".format(port)])
