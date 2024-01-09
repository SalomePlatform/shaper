#!/usr/bin/env python
"""Un exemple de création d'un rectangle"""

from salome.shaper import model
from salome.shaper import geom


model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))


### Rectangle by corners defined with doubles.
rectWithFloats = Sketch_1.addRectangle(-8, -8, -5, -3)
############################################################


### Rectangle by corners defined with SketchPoints.
SP_corner1 = Sketch_1.addPoint(-8, 8)
SP_corner2 = Sketch_1.addPoint(-5, 3)

rectWithSPs = Sketch_1.addRectangle(SP_corner1, SP_corner2)
############################################################


### Rectangle by corners defined with Pnt2Ds.
Pnt2D_corner1 = geom.Pnt2d(-8 - 5, 8)
Pnt2D_corner2 = geom.Pnt2d(-5 - 5, 3)

rectWithPnt2Ds = Sketch_1.addRectangle(Pnt2D_corner1, Pnt2D_corner2)
############################################################


############################################################
############################################################
### Rectangle by center and corner defined with Pnt2Ds.
Pnt2D_center = geom.Pnt2d(6.5 + 5, 5.5)
Pnt2D_corner = geom.Pnt2d(8.0 + 5, 8.0)

rectCenteredWithPnt2Ds, rectCenteredWithPnt2DCenterSketchPoint = Sketch_1.addRectangleCentered(Pnt2D_center, Pnt2D_corner)
############################################################


### Rectangle by center and corner defined with SketchPoints.
SP_center = Sketch_1.addPoint(6.5, 5.5)
SP_corner = Sketch_1.addPoint(8.0, 8.0)

rectCenteredWithPnt2Ds, _ = Sketch_1.addRectangleCentered(SP_center, SP_corner)
############################################################


### Rectangle by center and corner defined with doubles.
rectCenteredWithDoubles, _ = Sketch_1.addRectangleCentered(6.5, -5.5, 8.0, -8.0)
############################################################

model.do()
model.end()