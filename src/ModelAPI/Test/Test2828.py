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

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "a", "10")
model.addParameter(Part_1_doc, "b", "20")
ParamL = model.addParameter(Part_1_doc, "l", "100")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_1 = Sketch_1.addLine(13.37522515430284, 1.633845843958015, -6.624774845697162, 1.633845843958015)
SketchLine_2 = Sketch_1.addLine(-6.624774845697162, 1.633845843958015, -6.624774845697162, -8.366154156041985)
SketchLine_3 = Sketch_1.addLine(-6.624774845697162, -8.366154156041985, 13.37522515430284, -8.366154156041985)
SketchLine_4 = Sketch_1.addLine(13.37522515430284, -8.366154156041985, 13.37522515430284, 1.633845843958015)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), "a")
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_1.result(), "b")
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), "l", 0)
model.do()
from ModelAPI import *
aSession = ModelAPI_Session.get()
aSession.blockAutoUpdate(True)
model.testResultsVolumes(Extrusion_1, [20000])
ParamL.setValue(10)
# emulate the "See preview" press during the transaction opened
from ModelAPI import *
aSession = ModelAPI_Session.get()
aSession.blockAutoUpdate(False)
aSession.blockAutoUpdate(True)
# check that extrusion was updated
model.testResultsVolumes(Extrusion_1, [2000])
aSession.blockAutoUpdate(False)
model.end()
