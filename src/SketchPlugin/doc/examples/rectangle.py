from salome.shaper import model
from salome.shaper import geom

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
Rectangle_1 = Sketch_1.addRectangle(5.5, 8.5, 31.3, 78.9)
# rectangle from center and end points
center = geom.Pnt2d(10, 5)
corner = geom.Pnt2d(25, 75)
rectangle_2 = sketch.addRectangleCentered(center, corner)

model.do()
model.end()
