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
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Polyline_1_objects = [model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Top]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Top]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Bottom]"), model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Right][Box_1_1/Bottom]"), model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Right][Box_1_1/Top]")]
Polyline_1 = model.addPolyline3D(Part_1_doc, Polyline_1_objects, False)
model.do()

SubShapes_1 = model.addSubShapes(Part_1_doc, model.selection("WIRE", "Polyline_1_1"), [model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]")])
assert(SubShapes_1.feature().error() != "")
Part_1_doc.removeFeature(SubShapes_1.feature())

SubShapes_1 = model.addSubShapes(Part_1_doc, model.selection("WIRE", "Polyline_1_1"), [model.selection("EDGE", "Polyline_1_1/Generated_Edge&weak_name_4")])
assert(SubShapes_1.feature().error() == "")

model.end()

from ModelAPI import *
aSession = ModelAPI_Session.get()

aSession.startOperation()
SubShapes_2 = featureToCompositeFeature(Part_1.feature()).addFeature("SubShapes")
SubShapes_2.execute()
assert(SubShapes_2.error() == "")
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

SubShapes_1 = model.addSubShapes(Part_1_doc, model.selection("FACE", "Sketch_1/Face-SketchLine4r-SketchLine3r-SketchLine2r-SketchLine1r"), [model.selection("EDGE", "Sketch_1/SketchLine_1")])
assert(SubShapes_1.feature().error() != "")
Part_1_doc.removeFeature(SubShapes_1.feature())

SubShapes_1 = model.addSubShapes(Part_1_doc, model.selection("WIRE", "Polyline_1_1"), [model.selection("EDGE", "Sketch_1/SketchLine_1")])
assert(SubShapes_1.feature().error() != "")
Part_1_doc.removeFeature(SubShapes_1.feature())

SubShapes_1 = model.addSubShapes(Part_1_doc, model.selection("WIRE", "Polyline_1_1"), [model.selection("VERTEX", "Sketch_1/SketchLine_1_StartVertex")])
assert(SubShapes_1.feature().error() != "")
Part_1_doc.removeFeature(SubShapes_1.feature())

model.end()
