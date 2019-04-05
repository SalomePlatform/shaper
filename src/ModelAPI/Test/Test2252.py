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

from salome.shaper import model
from ModelAPI import *
from GeomAlgoAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(60, 0, 0, 0)
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 60)
SketchLine_3 = Sketch_1.addLine(0, 60, 60, 60)
SketchLine_4 = Sketch_1.addLine(60, 60, 60, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), 60)
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 90, 0)
model.do()
aSession = ModelAPI_Session.get()
aSession.setActiveDocument(partSet, False)
Translation_1 = model.addTranslation(partSet, [model.selection("COMPOUND", "Part_1/")], model.selection("EDGE", "OX"), 100)
model.do()
# move timeline before Translation
partSet.setCurrentFeature(Part_1.feature(), True)
Rotation_1 = model.addRotation(partSet, [model.selection("COMPOUND", "Part_1/")], model.selection("EDGE", "OY"), 90)
model.do()
# move timeline to the end of history
partSet.setCurrentFeature(Translation_1.feature(), True)
model.end()

# check that rotation then translation were applied consequently
assert(partSet.size("Parts") == 1)
resShape = modelAPI_Result(partSet.object("Parts", 0)).shape()
# height of extrusion 90 / 2 + translation 100
assert(GeomAlgoAPI_ShapeTools_centreOfMass(resShape).x() == 145)
# rotated
assert(floor(GeomAlgoAPI_ShapeTools_centreOfMass(resShape).z()) == -30)

assert(model.checkPythonDump())
