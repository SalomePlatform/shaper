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
ParamA = model.addParameter(Part_1_doc, "a", "200")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(19.66215729983772, 107.3479424462301, -0.3378427001622813, 107.3479424462301)
SketchLine_2 = Sketch_1.addLine(-0.3378427001622813, 107.3479424462301, -0.3378427001622813, 467.3479424462301)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-0.3378427001622813, 467.3479424462301, 49.66215729983772, 467.3479424462301)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-0.3378427001622813, 307.3479424462301, 29.66215729983772, 307.3479424462301)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_4.startPoint(), SketchLine_2.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), 50)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_4.result(), 30)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_1.result(), 20)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_1.endPoint(), SketchLine_4.startPoint(), "a")
SketchConstraintDistanceVertical_2 = Sketch_1.setVerticalDistance(SketchLine_4.startPoint(), SketchLine_2.endPoint(), "a*0.8")
model.do()
Edge_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_1")]
Edge_1 = model.addEdge(Part_1_doc, Edge_1_objects)
Revolution_1_objects = [model.selection("EDGE", "Edge_1_1"), model.selection("EDGE", "Edge_1_2"), model.selection("EDGE", "Edge_1_3")]
Revolution_1 = model.addRevolution(Part_1_doc, Revolution_1_objects, model.selection("EDGE", "Sketch_1/SketchLine_2"), 360, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Revolution_1_2"))
SketchCircle_1 = Sketch_2.addCircle(-8.007012574805483, 5.997620437244276, 8.148502128958617)
model.do()
# change the height of the second sketch plane
ParamA.setValue(50)
model.end()
# check that the second sketch result is recomputed: is located in the same plane as base
referenceY = Sketch_2.feature().selection("External").value().middlePoint().y()
resultY = Sketch_2.feature().results()[0].shape().middlePoint().y()
assert(referenceY == resultY)
