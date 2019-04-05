# Copyright (C) 2018-2019  CEA/DEN, EDF R&D
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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-14.48008258397498, -13.79591274114419, 5.519917416025022, -13.79591274114419)
SketchLine_2 = Sketch_1.addLine(10.51991741602502, -8.795912741144186, 10.51991741602502, 18.20408725885581)
SketchLine_3 = Sketch_1.addLine(10.51991741602502, 18.20408725885581, -14.48008258397498, 13.79591274114419)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_1.setName("SketchConstraintCoincidence_2")
SketchLine_4 = Sketch_1.addLine(-14.48008258397498, 13.79591274114419, -14.48008258397498, -13.79591274114419)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_3")
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
SketchConstraintCoincidence_3.setName("SketchConstraintCoincidence_4")
SketchArc_1 = Sketch_1.addArc(5.519917416025022, -8.795912741144186, 5.519917416025022, -13.79591274114419, 10.51991741602502, -8.795912741144186, False)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_1.endPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_5")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_6")
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_1.result())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_2.result())
SketchCircle_1 = Sketch_1.addCircle(0, 0, 5)
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_4.result(), SketchLine_3.result(), 100)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 27)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_1.startPoint(), SketchLine_2.result(), 25, True)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 5)
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_1.result(), SketchLine_4.result())
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_4.startPoint(), SketchCircle_1.center(), 20, True)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_4.endPoint(), SketchCircle_1.center(), 20, True)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchCircle_1.center())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_7")
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchCircle_1.results()[1], 5)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchArc_1_2f-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchCircle_1_2r")], model.selection(), 10, 0)
model.do()
# create a drawer feature of the extrusion and upper face, without dimensions
Drawer1 = Part_1_doc.addFeature("SketchDrawer")
Drawer1.selection("base_shape").setValue(Extrusion_1.feature(), None)
Drawer1.boolean("add_dimensions").setValue(False)
PlaneSelection = model.selection("FACE", "Extrusion_1_1/To_Face")
PlaneSelection.fillAttribute(Drawer1.selection("plane"))
model.do()
# create a drawer feature of the extrusion and upper face, with dimensions
Drawer2 = Part_1_doc.addFeature("SketchDrawer")
Drawer2.selection("base_shape").setValue(Extrusion_1.feature(), None)
Drawer2.boolean("add_dimensions").setValue(True)
PlaneSelection = model.selection("FACE", "Extrusion_1_1/To_Face")
PlaneSelection.fillAttribute(Drawer2.selection("plane"))
model.end()

# check that a sketch is created with valid elements
assert(Part_1_doc.size("Construction") == 3) # three sketches: base of extrusion and drawer's two

from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()

Sketch1 = objectToFeature(Part_1_doc.object("Features", 2)) # the first created sketch feature
assert(aFactory.validate(Sketch1))
assert(len(Sketch1.results()) == 1)

Sketch2 = objectToFeature(Part_1_doc.object("Features", 3)) # the second created sketch feature
assert(aFactory.validate(Sketch2))
assert(len(Sketch2.results()) == 1)
