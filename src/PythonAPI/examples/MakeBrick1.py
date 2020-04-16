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

# Creation of a box using the end-user API
# Author: Daniel Brunier-Coulin
# -----------------------------

from salome.shaper import model


# Initialisation

model.begin()
mypartset = model.moduleDocument()


# Creating a new Part

mypart = model.addPart(mypartset).document()


# Creating the base of the box

mybase = model.addSketch(mypart, model.defaultPlane("XOY"))

l1 = mybase.addLine(0, 0, 0, 25)
l2 = mybase.addLine(0, 25, 25, 25)
l3 = mybase.addLine(25, 25, 25, 0)
l4 = mybase.addLine(25, 0, 0, 0)

mybase.setCoincident(l1.endPoint(), l2.startPoint())
mybase.setCoincident(l2.endPoint(), l3.startPoint())
mybase.setCoincident(l3.endPoint(), l4.startPoint())
mybase.setCoincident(l4.endPoint(), l1.startPoint())

mybase.setParallel(l1, l3)
mybase.setParallel(l2, l4)

mybase.setPerpendicular(l1, l4)

mybase.setVertical(l1)
mybase.setFixed(l1.startPoint())

mywidth = mybase.setLength(l1, 50)
mylength = mybase.setDistance(l1.startPoint(), l3, 50)
model.do()

# Creating the extrusion

mybox = model.addExtrusion(mypart, mybase.selectFace(), 50)
model.do()

# Creating a cylinder on a face of the box

thisface = "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2"
thisxmax = "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1]"
thiszmax = "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/To_Face]"

mystand = model.addSketch(mypart, thisface)

c1 = mystand.addCircle(0, 25, 5)
l1 = mystand.addLine(thisxmax)
l2 = mystand.addLine(thiszmax)
model.do()
mystand.setDistance(c1.center(), l1, 10)
mystand.setDistance(c1.center(), l2, 10)
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
mybox.setSize(80)
model.end()

assert(model.checkPythonDump())
