# Creation of a box using the end-user API
# Author: Daniel Brunier-Coulin
# -----------------------------

import model
import geom


# Initialisation

model.begin()
mypartset = model.moduleDocument()


# Creating a new Part

mypart = model.addPart(mypartset).document()


# Creating the base of the box

mybase = model.addSketch(mypart, model.defaultPlane("XOY"))

p1 = geom.Pnt2d(0, 0)
p2 = geom.Pnt2d(0, 1)
p3 = geom.Pnt2d(1, 1)
p4 = geom.Pnt2d(1, 0)

line = mybase.addPolygon(p1, p2, p3, p4)

mybase.setParallel(line[0].result(), line[2].result())
mybase.setParallel(line[1].result(), line[3].result())
mybase.setPerpendicular(line[0].result(), line[3].result())

mywidth = mybase.setLength(line[0].result(), 50)
mylength = mybase.setDistance(line[0].startPointData(), line[2].result(), 50)


# Creating the extrusion

mybox = model.addExtrusion(mypart, mybase.selectFace(), 50)


# Creating a cylinder on a face of the box

thisface = "Extrusion_1_1/LateralFace_2"
thisxmin = "Extrusion_1_1/LateralFace_3|Extrusion_1_1/LateralFace_2"
thiszmax = "Extrusion_1_1/LateralFace_2|Extrusion_1_1/ToFace_1"

mystand = model.addSketch(mypart, thisface)
circle = mystand.addCircle(0, 25, 5)
mystand.setDistance(circle.centerData(), thisxmin, 10)
mystand.setDistance(circle.centerData(), thiszmax, 10)

myboss = model.addExtrusion(mypart, mystand.selectFace(), -5)


# Subtracting the cylinder to the box

model.addSubtraction(mypart, mybox.result(), myboss.result())
model.end()


# Editing the box

model.begin()
mybase.setValue(mylength, 100)
mybox.setSize(20)
model.end()
