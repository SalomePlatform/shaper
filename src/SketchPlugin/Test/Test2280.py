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

"""
    Test2280.py
    Test case for issue #2280 "Trim in the sketch generates a SIGSEGV"
"""

from salome.shaper import model
from GeomAPI import GeomAPI_Pnt2d

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Cylinder_1_1/Face_2"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "Cylinder_1_1/Face_1&Cylinder_1_1/Face_2"), True)
SketchCircle_1 = SketchProjection_1.createdFeature()
SketchCircle_2 = Sketch_1.addCircle(0.0, 10.0, 8.0)
model.do()

assert(Sketch_1.feature().error() == "")
assert(Sketch_1.solverError().value() == "")
model.testNbSubFeatures(Sketch_1, "SketchArc", 0)
model.testNbSubFeatures(Sketch_1, "SketchCircle", 2)

# Trim sircle
SketchTrim_1 = Sketch_1.addTrim(SketchCircle_2, GeomAPI_Pnt2d(0.0, 18.0))
model.do()

assert(Sketch_1.feature().error() == "")
assert(Sketch_1.solverError().value() == "")
model.testNbSubFeatures(Sketch_1, "SketchArc", 1)
model.testNbSubFeatures(Sketch_1, "SketchCircle", 1)

model.end()
