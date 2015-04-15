# Creation of a box using the end-user API
# Author: Daniel Brunier-Coulin
# -----------------------------

import modeler


# Initialisation

modeler.begin()
mypartset = modeler.moduleDocument()


# Creating a new Part

mypart = modeler.addPart(mypartset).document()


# Creating the base of the box

mybase = modeler.addSketch( mypart, modeler.defaultPlane("XOY") )

l1 = mybase.addLine( 0, 0, 0, 1 )
l2 = mybase.addLine( 0, 1, 1, 1 )
l3 = mybase.addLine( 1, 1, 1, 0 )
l4 = mybase.addLine( 1, 0, 0, 0 )

mybase.setCoincident( l1.endPointData(), l2.startPointData() )
mybase.setCoincident( l2.endPointData(), l3.startPointData() )
mybase.setCoincident( l3.endPointData(), l4.startPointData() )
mybase.setCoincident( l4.endPointData(), l1.startPointData() )

mybase.setParallel( l1.result(), l3.result() )
mybase.setParallel( l2.result(), l4.result() )

mybase.setPerpendicular( l1.result(), l4.result() )

mywidth  = mybase.setLength( l1.result(), 50 )
mylength = mybase.setDistance( l1.startPointData(), l3.result(), 50 )


# Creating the extrusion

mybox = modeler.addExtrusion( mypart, mybase.selectFace(), 50 )


# Creating a cylinder on a face of the box

thisface = "Extrusion_1/LateralFace_2"
thisxmin = "Extrusion_1/LateralFace_3|Extrusion_1/LateralFace_2"
thisxmax = "Extrusion_1/LateralFace_2|Extrusion_1/LateralFace_1"
thiszmin = "Sketch_1/Edge5_1"
thiszmax = "Extrusion_1/LateralFace_2|Extrusion_1/TopFace_1"

mystand = modeler.addSketch( mypart, thisface )
c1      = mystand.addCircle( 0, 25, 5)
mystand.setDistance( c1.centerData(), thisxmin, 10 )
mystand.setDistance( c1.centerData(), thiszmax, 10 )

myboss = modeler.addExtrusion( mypart, mystand.selectFace(c1.result()), -5 )


# Subtracting the cylinder to the box

modeler.addSubtraction( mypart, mybox.result(), myboss.result() )
modeler.end()


# Editing the box

modeler.begin()
mybase.setValue( mylength, 100 )
mybox.setSize( 20 )
modeler.end()
