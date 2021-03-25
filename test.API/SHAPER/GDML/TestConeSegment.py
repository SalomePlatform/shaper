# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
Test case for GDML Primitive Cone Segment feature.
Written on High API.
"""
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Parameters
model.addParameter(Part_1_doc, "rmin1", "7")
model.addParameter(Part_1_doc, "rmax1", "11")
model.addParameter(Part_1_doc, "rmin2", "5")
model.addParameter(Part_1_doc, "rmax2", "8")
model.addParameter(Part_1_doc, "z", "12")
model.addParameter(Part_1_doc, "startphi", "0")
model.addParameter(Part_1_doc, "deltaphi", "270")

CS1 = model.addConeSegment(Part_1_doc, 7, 11, 5, 8, 12, 0 , 270)
CS2 = model.addConeSegment(Part_1_doc, 5, 10, 7, 15, 15, 45 , 180)
CS3 = model.addConeSegment(Part_1_doc, 5, 10, 7, 15, 15, -45 , 180)
CS4 = model.addConeSegment(Part_1_doc, 0, 10, 2, 7, 3, 0 , 270)
CS5 = model.addConeSegment(Part_1_doc, 5, 8, 0, 6, 10, 45 , 270)
CS6 = model.addConeSegment(Part_1_doc, 0, 6, 0, 4, 17, 0 , 270)
CS7 = model.addConeSegment(Part_1_doc, 7, 11, 5, 8, 12, 0 , 360)
CS8 = model.addConeSegment(Part_1_doc, -5, 11, 5, 8, 12, 0 , 270)
CS9 = model.addConeSegment(Part_1_doc, 11, 7, 5, 8, 12, 0 , 270)
CS10 = model.addConeSegment(Part_1_doc, 7, 11, 8, 5, 12, 0 , 270)
CS11 = model.addConeSegment(Part_1_doc, 7, 11, -3, 5, 12, 0 , 270)
CS12 = model.addConeSegment(Part_1_doc, 7, 11, 5, 8, 0, 0 , 270)
CS13 = model.addConeSegment(Part_1_doc, 7, 11, 5, 8, -10, 0 , 270)
CS14 = model.addConeSegment(Part_1_doc, 7, 11, 5, 8, 12, -45 , -45)
CS15 = model.addConeSegment(Part_1_doc, 7, 11, 5, 8, 12, 45 , 450)
CS16 = model.addConeSegment(Part_1_doc, "rmin1", "rmax1", "rmin2", "rmax2", "z", "startphi" , "deltaphi")
model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(CS1, 1)
model.testNbSubResults(CS1, [0])
model.testNbSubShapes(CS1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(CS1, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(CS1, model, Part_1_doc)

model.testNbResults(CS2, 1)
model.testNbSubResults(CS2, [0])
model.testNbSubShapes(CS2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(CS2, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(CS2, model, Part_1_doc)

model.testNbResults(CS3, 1)
model.testNbSubResults(CS3, [0])
model.testNbSubShapes(CS3, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(CS3, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(CS3, model, Part_1_doc)

model.testNbResults(CS4, 1)
model.testNbSubResults(CS4, [0])
model.testNbSubShapes(CS4, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(CS4, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(CS4, model, Part_1_doc)

model.testNbResults(CS5, 1)
model.testNbSubResults(CS5, [0])
model.testNbSubShapes(CS5, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(CS5, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(CS5, model, Part_1_doc)

model.testNbResults(CS6, 1)
model.testNbSubResults(CS6, [0])
model.testNbSubShapes(CS6, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(CS6, GeomAPI_Shape.FACE, [5])
model.testHaveNamingFaces(CS6, model, Part_1_doc)

model.testNbResults(CS7, 1)
model.testNbSubResults(CS7, [0])
model.testNbSubShapes(CS7, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(CS7, GeomAPI_Shape.FACE, [4])
model.testHaveNamingFaces(CS7, model, Part_1_doc)

model.testNbResults(CS16, 1)
model.testNbSubResults(CS16, [0])
model.testNbSubShapes(CS16, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(CS16, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(CS16, model, Part_1_doc)

model.testNbResults(CS8, 0)
assert(CS8.feature().error() == "Cone Segment builder :: rmin1 is negative.")

model.testNbResults(CS9, 0)
assert(CS9.feature().error() == "Cone Segment builder :: rmin1 is larger than rmax1.")

model.testNbResults(CS10, 0)
assert(CS10.feature().error() == "Cone Segment builder :: rmin2 is larger than rmax2.")

model.testNbResults(CS11, 0)
assert(CS11.feature().error() == "Cone Segment builder :: rmin2 is negative.")

model.testNbResults(CS12, 0)
assert(CS12.feature().error() == "Cone Segment builder :: z is negative or null.")

model.testNbResults(CS13, 0)
assert(CS13.feature().error() == "Cone Segment builder :: z is negative or null.")

model.testNbResults(CS14, 0)
assert(CS14.feature().error() == "Cone Segment builder :: deltaphi is negative or null.")

model.testNbResults(CS15, 0)
assert(CS15.feature().error() == "Cone Segment builder :: deltaphi is larger than 360 degree.")