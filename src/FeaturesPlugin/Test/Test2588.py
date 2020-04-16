# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchCircle_1 = Sketch_1.addCircle(0, 0, 2)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 2)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, 0)
Symmetry_1 = model.addSymmetry(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_1")], model.selection("VERTEX", "Sketch_1/SketchCircle_1_2__cc"), True)
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("COMPOUND", "all-in-Symmetry_1")], model.selection("EDGE", "PartSet/OY"), 5, 3)
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("COMPOUND", "all-in-LinearCopy_1")], model.selection("EDGE", "PartSet/OZ"), 45, 8)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbSubShapes(AngularCopy_1, GeomAPI_Shape.SOLID, [48])

assert(model.checkPythonDump())
