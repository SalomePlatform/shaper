## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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

# This test is based on the issue 1799: after faces are splitted and group is moved
# down, the group-faces also must be splitted, the AttributeSelectionList should contain
# 4 times more faces.
# The biggest part of the code is python dump of the updated_for_1799.hdf file from this issue.

from salome.shaper import model
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(350, 250, 1.524660639134633e-014, 250)
SketchLine_2 = Sketch_1.addLine(-350, 1.720151798778602e-014, -350, -250)
SketchLine_3 = Sketch_1.addLine(-350, -250, 1.524660628546721e-014, -250)
SketchLine_4 = Sketch_1.addLine(350, -250, 350, 1.720151798778602e-014)
SketchLine_5 = Sketch_1.addLine(350, 1.720151798778602e-014, 350, 250)
SketchLine_5.setName("SketchLine_7")
SketchLine_5.result().setName("SketchLine_7")
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_1.startPoint())
SketchLine_6 = Sketch_1.addLine(1.524660639134633e-014, 250, -350, 250)
SketchLine_6.setName("SketchLine_5")
SketchLine_6.result().setName("SketchLine_5")
SketchLine_7 = Sketch_1.addLine(-350, 250, -350, 1.720151798778602e-014)
SketchLine_7.setName("SketchLine_6")
SketchLine_7.result().setName("SketchLine_6")
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_8 = Sketch_1.addLine(1.524660628546721e-014, -250, 350, -250)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchCircle_1 = Sketch_1.addCircle(0, 0, 101.5814068355669)
SketchPoint_1 = Sketch_1.addPoint(1.524660639134633e-014, 250)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_1.endPoint())
SketchPoint_2 = Sketch_1.addPoint(-350, 1.720151798778602e-014)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_2.startPoint())
SketchPoint_3 = Sketch_1.addPoint(1.524660628546721e-014, -250)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchPoint_3.coordinates(), SketchLine_3.endPoint())
SketchPoint_4 = Sketch_1.addPoint(101.5814068355669, 1.720151798778602e-014)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchPoint_4.coordinates(), SketchCircle_1.results()[1])
SketchPoint_5 = Sketch_1.addPoint(-101.5814068355669, 1.720151798778602e-014)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchPoint_5.coordinates(), SketchCircle_1.results()[1])
SketchPoint_6 = Sketch_1.addPoint(1.524660639134633e-014, 101.5814068355669)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchPoint_6.coordinates(), SketchCircle_1.results()[1])
SketchPoint_7 = Sketch_1.addPoint(1.524660628546721e-014, -101.5814068355669)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchPoint_7.coordinates(), SketchCircle_1.results()[1])
SketchPoint_8 = Sketch_1.addPoint(350, 1.720151798778602e-014)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchPoint_8.coordinates(), SketchLine_4.endPoint())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_6.startPoint())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_1.result(), SketchLine_6.result())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_2.startPoint())
#SketchConstraintParallel_2 = Sketch_1.setParallel(SketchLine_2.result(), SketchLine_7.result())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintParallel_3 = Sketch_1.setParallel(SketchLine_4.result(), SketchLine_5.result())
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_8.startPoint())
SketchConstraintParallel_4 = Sketch_1.setParallel(SketchLine_3.result(), SketchLine_8.result())
SketchLine_9 = Sketch_1.addLine(1.524660639134633e-014, 250, 1.524660639134633e-014, 101.5814068355669)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_9.startPoint())
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchPoint_6.coordinates(), SketchLine_9.endPoint())
SketchLine_10 = Sketch_1.addLine(-101.5814068355669, 1.720151798778602e-014, -350, 1.720151798778602e-014)
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchPoint_5.coordinates(), SketchLine_10.startPoint())
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_10.endPoint())
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_10.endPoint())
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_10.endPoint())
SketchLine_11 = Sketch_1.addLine(1.524660628546721e-014, -250, 1.524660628546721e-014, -101.5814068355669)
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchPoint_3.coordinates(), SketchLine_11.startPoint())
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchLine_8.startPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchPoint_7.coordinates(), SketchLine_11.endPoint())
SketchLine_12 = Sketch_1.addLine(350, 1.720151798778602e-014, 101.5814068355669, 1.720151798778602e-014)
SketchConstraintCoincidence_29 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_30 = Sketch_1.setCoincident(SketchPoint_8.coordinates(), SketchLine_12.startPoint())
SketchConstraintCoincidence_31 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_12.startPoint())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_11.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_10.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_12.result())
SketchConstraintCoincidence_32 = Sketch_1.setCoincident(SketchLine_9.result(), SketchLine_11.endPoint())
SketchConstraintCoincidence_33 = Sketch_1.setCoincident(SketchLine_10.result(), SketchLine_12.startPoint())
SketchConstraintCoincidence_34 = Sketch_1.setCoincident(SketchPoint_4.coordinates(), SketchLine_12.endPoint())
SketchPoint_9 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintCoincidence_35 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchPoint_9.coordinates())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_6.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_8.result())
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_10.result(), SketchLine_12.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_6.result(), 350)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_7.result(), 250)
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchLine_9.result(), SketchLine_11.result())
SketchConstraintEqual_5 = Sketch_1.setEqual(SketchLine_7.result(), SketchLine_2.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_5f-SketchLine_6f-SketchLine_10r-SketchCircle_1_2r-SketchLine_9r"), model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_9f-SketchCircle_1_2r-SketchLine_12r-SketchLine_7f"), model.selection("FACE", "Sketch_1/Face-SketchLine_4f-SketchLine_12f-SketchCircle_1_2r-SketchLine_11r-SketchLine_8f"), model.selection("FACE", "Sketch_1/Face-SketchLine_2f-SketchLine_3f-SketchLine_11f-SketchCircle_1_2r-SketchLine_10f")], model.selection(), 150, 0)
Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "Extrusion_1_1_1/To_Face"), model.selection("FACE", "Extrusion_1_1_4/To_Face"), model.selection("FACE", "Extrusion_1_1_3/To_Face"), model.selection("FACE", "Extrusion_1_1_2/To_Face")])
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_6"), 150, True)
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchLine_4"), 150, True)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("FACE", "Plane_1"), model.selection("COMPSOLID", "Extrusion_1_1"), model.selection("FACE", "Plane_2")])
Plane_6 = model.addPlane(Part_1_doc, model.selection("FACE", "Partition_1_1_7/Modified_Face&Sketch_1/SketchLine_3"), 110, True)
Plane_7 = model.addPlane(Part_1_doc, model.selection("FACE", "Partition_1_1_5/Modified_Face&Sketch_1/SketchLine_8"), 380, True)
Partition_2 = model.addPartition(Part_1_doc, [model.selection("FACE", "Plane_3"), model.selection("COMPSOLID", "Partition_1_1"), model.selection("FACE", "Plane_4")])
model.do()

# check that group contains only 4 faces
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 4)
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Group_1.feature()))
# move group to the end: it must be recomputed, faces are splitted
Part_1_doc.moveFeature(Group_1.feature(), Partition_2.feature())
model.end()

# check that number of faces becomes 4*4=16 and Group is valid
assert(selectionList.size() == 16)
assert(aFactory.validate(Group_1.feature()))
