# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

from salome.shaper import model
from ModelAPI import *
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(15, 13, 11)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, 0)
model.do()

Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Sketch_2 = model.addSketch(Part_2_doc, model.defaultPlane("XOZ"))
SketchCircle_2 = Sketch_2.addCircle(-13, -9, 5)
model.do()
Extrusion_2 = model.addExtrusion(Part_2_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, 0)
ImportResult_1 = model.addImportResult(Part_2_doc, [model.selection("SOLID", "Part_1/Extrusion_1_1")])
model.do()

Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()
Extrusion_3 = model.addExtrusion(Part_3_doc, [], model.selection(), 1, 0)
Sketch_3 = model.addSketch(Part_3_doc, model.defaultPlane("XOZ"))
SketchCircle_3 = Sketch_3.addCircle(-4, 2, 5)
Extrusion_3.setNestedSketch(Sketch_3)
model.do()

model.end()

# check Part_2 is ok and contains 2 solids in result
assert(Part_2.feature().results().size() == 1)
aPartShape = Part_2.feature().firstResult().shape()
assert(aPartShape.shapeTypeStr() == "COMPOUND")
aShapeExplorer = GeomAPI_ShapeExplorer(aPartShape, GeomAPI_Shape.SOLID)
assert(aShapeExplorer.more())
aShapeExplorer.next()
assert(aShapeExplorer.more())

# check that selection of this part and the lower part is impossible (by validator)
model.begin()
aSel = ImportResult_1.feature().selectionList("objects").value(0)
aSel.selectSubShape("SOLID", "Part_2/Extrusion_1_1")
model.end()
aFactory = ModelAPI_Session.get().validators()
assert(not aFactory.validate(ImportResult_1.feature()))

model.begin()
aSel = ImportResult_1.feature().selectionList("objects").value(0)
aSel.selectSubShape("SOLID", "Part_3/Extrusion_1_1")
model.end()
assert(not aFactory.validate(ImportResult_1.feature()))

# back to correct value
model.begin()
aSel = ImportResult_1.feature().selectionList("objects").value(0)
aSel.selectSubShape("SOLID", "Part_1/Extrusion_1_1")
model.end()
assert(aFactory.validate(ImportResult_1.feature()))

# TODO: implement for GEOMOETRICAL also
assert(model.checkPythonDump(model.ModelHighAPI.CHECK_NAMING))
