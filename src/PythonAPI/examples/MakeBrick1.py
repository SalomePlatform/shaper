# Creation of a box using the end-user API
# Author: Daniel Brunier-Coulin
# -----------------------------

import model


# Initialisation

model.begin()
mypartset = model.moduleDocument()


# Creating a new Part

mypart = model.addPart(mypartset).document()


# Creating the base of the box

mybase = model.addSketch(mypart, model.defaultPlane("XOY"))

l1 = mybase.addLine(0, 0, 0, 1)
l2 = mybase.addLine(0, 1, 1, 1)
l3 = mybase.addLine(1, 1, 1, 0)
l4 = mybase.addLine(1, 0, 0, 0)

mybase.setCoincident(l1.endPoint(), l2.startPoint())
mybase.setCoincident(l2.endPoint(), l3.startPoint())
mybase.setCoincident(l3.endPoint(), l4.startPoint())
mybase.setCoincident(l4.endPoint(), l1.startPoint())

mybase.setParallel(l1.result(), l3.result())
mybase.setParallel(l2.result(), l4.result())

mybase.setPerpendicular(l1.result(), l4.result())

mywidth = mybase.setLength(l1.result(), 50)
mylength = mybase.setDistance(l1.startPoint(), l3.result(), 50)

# Creating the extrusion

mybox = model.addExtrusion(mypart, mybase.selectFace(), 50)

# Creating a cylinder on a face of the box

thisface = "Extrusion_1_1/Generated_Face_2"
thisxmin = "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_2"
thisxmax = "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/Generated_Face_1"
thiszmin = "Sketch_1/Edge5_1"
thiszmax = "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/To_Face_1_1"

mystand = model.addSketch(mypart, thisface)

c1 = mystand.addCircle(0, 25, 5)
mystand.setDistance(c1.center(), thisxmin, 10)
mystand.setDistance(c1.center(), thiszmax, 10)

myboss = model.addExtrusion(mypart, mystand.selectFace(c1.result()), -5)

# Subtracting the cylinder to the box

model.addSubtraction(mypart, mybox.result(), myboss.result())
model.end()


# Editing the box

model.begin()
mybase.setValue(mylength, 100)
mybox.setSize(80)
model.end()
