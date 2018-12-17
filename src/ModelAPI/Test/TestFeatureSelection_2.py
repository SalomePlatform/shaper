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

# The test for correct selection of a whole feautre as argument of other feature: select all extrusions
# produced from the scetch, then change the number of sketch contours, so, anytway all extrusions must
# still be selected in the Cut Boolean oepration

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Param_N = model.addParameter(Part_1_doc, "n", "3")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(30, 0, 15)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.result(), SketchCircle_1.center())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_1).startPoint(), SketchCircle_1.center(), 30, True)
SketchMultiRotation_1 = Sketch_1.addRotation([SketchCircle_1.results()[1]], SketchAPI_Line(SketchLine_1).startPoint(), 360, "n=", True)
[SketchCircle_2, SketchCircle_3] = SketchMultiRotation_1.rotated()
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 15)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchCircle_4 = Sketch_2.addCircle(25, 5, 4)
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_4.results()[1], 4)
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchLine_2.result(), SketchCircle_4.center(), 5, True)
SketchConstraintDistanceHorizontal_1 = Sketch_2.setHorizontalDistance(SketchAPI_Line(SketchLine_2).startPoint(), SketchCircle_4.center(), 25)
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_4_2r")], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_1")], [model.selection("COMPOUND", "all-in-Revolution_1")])
model.do()

# update the parameter N => number of sketch contours is changed
Param_N.setValue(4)
Part_1_doc.setCurrentFeature(None, True)
model.do()
Part_1_doc.setCurrentFeature(Cut_1.feature(), True)
model.end()

assert(Cut_1.result().numberOfSubs() == 4)

assert(model.checkPythonDump())
