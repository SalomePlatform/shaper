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

from SketchAPI import *
from GeomAlgoAPI import *
from GeomAPI import *
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "k", "30")
model.addParameter(Part_1_doc, "M", "10")
model.addParameter(Part_1_doc, "h", "20")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(15, -8.660254037843767, 15, 8.660254037843762)
SketchLine_2 = Sketch_1.addLine(15, 8.660254037843762, 0, 0)
SketchLine_2.setAuxiliary(True)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(0, 0, 15, -8.660254037843767)
SketchLine_3.setAuxiliary(True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_2.result(), SketchLine_1.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_1.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_3.startPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_3.startPoint(), SketchLine_1.result(), "k/2", True)
SketchMultiRotation_1 = Sketch_1.addRotation([SketchLine_1.result()], SketchLine_3.startPoint(), 360, 6, True)
[SketchLine_4, SketchLine_5, SketchLine_6, SketchLine_7, SketchLine_8] = SketchMultiRotation_1.rotated()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 10)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], "M")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_3.startPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_4f-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_8f-SketchCircle_1_2r")], model.selection(), "h", 0)
model.do()
# fillet with only one edge to  make correct result and put all edges to results sub-tree
Fillet_1_objects = [model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7]")]
Fillet_1 = model.addFillet(Part_1_doc, Fillet_1_objects, 2)
model.do()
# select all edges as a fillet arguments
aResult = Extrusion_1.results()[0].resultSubShapePair()[0]
aShape = aResult.shape()
aShapeExplorer = GeomAPI_ShapeExplorer(aShape, GeomAPI_Shape.EDGE)
aSelectionList = []
aLocations = {} # for unique locations support
while aShapeExplorer.more():
  # select only vertical lines
  anEdge = aShapeExplorer.current()
  if not anEdge.edge().isLine() or anEdge.edge().line().direction().z() < 0.999:
    aShapeExplorer.next()
    continue
  aLoc = anEdge.edge().line().location()
  aLocStr = str(aLoc.x()) + " " + str(aLoc.y()) + " " + str(aLoc.z())
  if aLocStr in aLocations:
    aShapeExplorer.next()
    continue
  aLocations[aLocStr] = ""
  aSelection = model.selection(aResult, aShapeExplorer.current()) # First argument should be result/sub-result, second is sub-shape on this result/sub-result
  aSelectionList.append(aSelection)
  aShapeExplorer.next()
# Set the fillet arguments: all edges
Fillet_1.setBase(aSelectionList)

model.end()

# if in the fillet base edges names the fillet is used, the python dump should fail
assert(model.checkPythonDump())
