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

# Test move the group in history for selection of a whole feature that produces compsolid

from salome.shaper import model
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-25.00123152709361, 27.48891625615764, 27.86206896551725, 27.48891625615764)
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchLine_2 = Sketch_1.addLine(27.86206896551725, 27.48891625615764, -23.88177339901478, -20.02586206896552)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-23.88177339901478, -20.02586206896552, 28.73275862068966, -20.02586206896552)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchLine_4 = Sketch_1.addLine(28.73275862068966, -20.02586206896552, 27.86206896551725, 27.48891625615764)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_4.endPoint())
SketchLine_5 = Sketch_1.addLine(-25.00123152709361, 27.48891625615764, 28.73275862068966, -20.02586206896552)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_5.endPoint())
model.do()
Extrusion_1_objects = [model.selection("FACE", "Sketch_1/Face-SketchLine_5f-SketchLine_2r-SketchLine_1r"), model.selection("FACE", "Sketch_1/Face-SketchLine_2f-SketchLine_5f-SketchLine_4f"), model.selection("FACE", "Sketch_1/Face-SketchLine_2f-SketchLine_3f-SketchLine_5r")]
Extrusion_1 = model.addExtrusion(Part_1_doc, Extrusion_1_objects, model.selection(), 10, 0)
# selection of a whole result: 5x3-2 faces (2 faces are shared)
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("COMPOUND", "all-in-Extrusion_1")])
model.testNbSubShapes(Group_1, GeomAPI_Shape.FACE, [13])
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 0, 10, [model.selection("COMPSOLID", "Extrusion_1_1")])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1_1/To_Face"))
SketchCircle_1 = Sketch_2.addCircle(13.4282260278248, 11.79859034244854, 3.718064241992405)
ExtrusionCut_1.setNestedSketch(Sketch_2)
model.do()
# move group after the extrusion-cut, so, it refers to it
Part_1_doc.moveFeature(Group_1.feature(), ExtrusionCut_1.feature())
model.end()

# check that there is a hole appeared in two solids: +4 faces (1 splitted planar, 3 faces of cylinder, divided by seam)
assert(len(Group_1.feature().results())==1)
model.testNbSubShapes(Group_1, GeomAPI_Shape.FACE, [17])

assert(model.checkPythonDump())
