## Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Point_2 = model.addPoint(Part_1_doc, 10, 0, 0)
Point_3 = model.addPoint(Part_1_doc, 20, 10, 0)
Point_4 = model.addPoint(Part_1_doc, 0, 20, 10)
Point_5 = model.addPoint(Part_1_doc, 30, 0, 10)
Polyline_1 = model.addPolyline3D(Part_1_doc, [model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2")], False)
Polyline_2 = model.addPolyline3D(Part_1_doc, [model.selection("VERTEX", "Point_3"), model.selection("VERTEX", "Point_4")], False)
model.do()

Filling_1 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Polyline_1_1/Generated_Edge&Point_1/Point_1"), model.selection("EDGE", "Polyline_2_1/Generated_Edge&Point_3/Point_3")], "curve_info", 9, 8, 0, 0.0001, 0.0001, True)
assert(Filling_1.feature().error() != "")
Part_1_doc.removeFeature(Filling_1.feature())

Filling_1 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Polyline_1_1/Generated_Edge&Point_1/Point_1")])
assert(Filling_1.feature().error() != "")
Part_1_doc.removeFeature(Filling_1.feature())

Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

Filling_1 = model.addFilling(Part_1_doc, [model.selection("FACE", "Box_1_1/Top")])
assert(Filling_1.feature().error() != "")
Part_1_doc.removeFeature(Filling_1.feature())

model.end()

from ModelAPI import *
aSession = ModelAPI_Session.get()

aSession.startOperation()
aFillingFeature = featureToCompositeFeature(Part_1.feature()).addFeature("Filling")
aFillingFeature.string("advanced_options").setValue("")
aFillingFeature.execute()
assert(aFillingFeature.error() != "")
aSession.finishOperation()

aSession.startOperation()
aBaseObjectsList = aFillingFeature.selectionList("base_objects")
aBaseObjectsList.append("Polyline_1_1/Generated_Edge&Point_1/Point_1", "EDGE")
aBaseObjectsList.append("Box_1_1/Top", "FACE")
aFillingFeature.execute()
assert(aFillingFeature.error() != "")
aSession.finishOperation()


model.begin()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(30, -30, -30, -30)
SketchLine_2 = Sketch_1.addLine(-30, -30, -30, 30)
SketchLine_3 = Sketch_1.addLine(-30, 30, 30, 30)
SketchLine_4 = Sketch_1.addLine(30, 30, 30, -30)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()

Filling_1 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", ""), model.selection("EDGE", "Sketch_1/SketchLine_1")], "auto_correct", 1, 1, 0, 0.0001, 0.0001, False)
assert(Filling_1.feature().error() != "")
Part_1_doc.removeFeature(Filling_1.feature())

Filling_1 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_1")], "auto_correct", 1, 1, 0, 0.0001, 0.0001, False)
assert(Filling_1.feature().error() != "")
model.end()
