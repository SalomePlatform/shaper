## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(60, -30, 15, -30)
SketchLine_2 = Sketch_1.addLine(15, -30, 15, 15)
SketchLine_3 = Sketch_1.addLine(15, 15, 60, 15)
SketchLine_4 = Sketch_1.addLine(60, 15, 60, -30)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)

Partition_1 = model.addPartition(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1")])
assert(Partition_1.feature().error() != "")
Part_1_doc.removeFeature(Partition_1.feature())

model.end()


from ModelAPI import *
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

aSession.startOperation()
Partition_1 = Part_1_doc.addFeature("Partition")
Partition_1.execute()
assert(Partition_1.error() != "")
aSession.finishOperation()
