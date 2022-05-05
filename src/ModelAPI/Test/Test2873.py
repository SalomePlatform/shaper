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

"""
    Test2873.py
    Test for issue #2873 "Freeze when inserting a new folder"
"""

from salome.shaper import model
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(50, -50, -50, -50)
SketchLine_2 = Sketch_1.addLine(-50, -50, -50, 50)
SketchLine_3 = Sketch_1.addLine(-50, 50, 50, 50)
SketchLine_4 = Sketch_1.addLine(50, 50, 50, -50)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")])
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Face_1_1/Modified_Edge&Sketch_1/SketchLine_4")])
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "PartSet/Origin")])
Folder_1 = model.addFolder(Part_1_doc, Face_1, Face_1)
Folder_2 = model.addFolder(Part_1_doc, Edge_1, Edge_1)
model.do()

FeaturesToFind = FeatureList()
FeaturesToFind.append(Sketch_1.feature())
Part_1_doc.removeFolder(Part_1_doc.findFolderBelow(FeaturesToFind))
model.do()

Folder_3 = model.addFolder(Part_1_doc, Vertex_1, Vertex_1)
model.do()
model.end()

assert(model.checkPythonDump())
