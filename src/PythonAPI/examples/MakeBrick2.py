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

# Creation of a box using the end-user API
# Author: Daniel Brunier-Coulin
# -----------------------------

from salome.shaper import model
from salome.shaper import geom


# Initialisation

model.begin()
mypartset = model.moduleDocument()


# Creating a new Part

mypart = model.addPart(mypartset).document()


# Creating the base of the box

mybase = model.addSketch(mypart, model.defaultPlane("XOY"))

p1 = geom.Pnt2d(0, 0)
p2 = geom.Pnt2d(0, 25)
p3 = geom.Pnt2d(25, 25)
p4 = geom.Pnt2d(25, 0)

line = model.addPolygon(mybase, p1, p2, p3, p4)

mybase.setParallel(line[0], line[2])
mybase.setParallel(line[1], line[3])
mybase.setPerpendicular(line[0], line[3])

mybase.setVertical(line[0])
mybase.setFixed(line[0].startPoint())

mywidth = mybase.setLength(line[0], 50)
mylength = mybase.setDistance(line[0].startPoint(), line[2], 50)
model.do()

# Creating the extrusion

mybox = model.addExtrusion(mypart, mybase.selectFace(), 50)
model.do()


# Creating a cylinder on a face of the box

thisface = "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2"
thisxmin = "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1]"
thiszmax = "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/To_Face]"

mystand = model.addSketch(mypart, thisface)
circle = mystand.addCircle(0, 25, 5)
mystand.setDistance(circle.center(), mystand.addLine(thisxmin), 10)
mystand.setDistance(circle.center(), mystand.addLine(thiszmax), 10)
model.do()

myboss = model.addExtrusion(mypart, mystand.selectFace(), -5)
model.do()

# Subtracting the cylinder to the box

model.addCut(mypart, mybox.results(), myboss.results())
model.end()


# Editing the box

model.begin()
mybase.setValue(mylength, 100)
model.do()
mybox.setSize(40)
model.end()

assert(model.checkPythonDump())
