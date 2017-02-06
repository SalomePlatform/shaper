"""
Test case for Primitive Box feature. 
Written on High API.
"""
from ModelAPI import *
from GeomAPI import *

from salome.shaper import model

# Get session
aSession = ModelAPI_Session.get()

# Create a part
aDocument = aSession.activeDocument()
aSession.startOperation()
model.addPart(aDocument)
aDocument = aSession.activeDocument()
aSession.finishOperation()

# Create a box with dimensions
aSession.startOperation()
aBox1 = model.addBox(aDocument, 10, 10, 10).result()
assert (aBox1 is not None)
print "box1 : ok"
aSession.finishOperation()

aSession.startOperation()
aBox2 = model.addBox(aDocument, 0, 10, 10)
assert (len(aBox2.results()) == 0)
print "box2 : ko (" + aBox2.feature().error() + ")"
aSession.finishOperation()

aSession.startOperation()
aBox3 = model.addBox(aDocument, 10, 0, 10)
assert (len(aBox3.results()) == 0)
print "box3 : ko (" + aBox3.feature().error() + ")"
aSession.finishOperation()

aSession.startOperation()
aBox4 = model.addBox(aDocument, 10, 10, 0)
assert (len(aBox4.results()) == 0)
print "box4 : ko (" + aBox4.feature().error() + ")"
aSession.finishOperation()

aSession.startOperation()
aBox5 = model.addBox(aDocument, -10, 10, 10)
assert (len(aBox5.results()) == 0)
print "box5 : ko (" + aBox5.feature().error() + ")"
aSession.finishOperation()

aSession.startOperation()
aBox6 = model.addBox(aDocument, 10, -10, 10)
assert (len(aBox6.results()) == 0)
print "box6 : ko (" + aBox6.feature().error() + ")"
aSession.finishOperation()

aSession.startOperation()
aBox7 = model.addBox(aDocument, 10, 10, -10)
assert (len(aBox7.results()) == 0)
print "box7 : ko (" + aBox7.feature().error() + ")"
aSession.finishOperation()

# Create a box with 2 points
aSession.startOperation()
aPoint1 = model.addPoint(aDocument, 0, 0, 0).result()
aPoint2 = model.addPoint(aDocument, 50, 50, 50).result()
aBox8 = model.addBox(aDocument, aPoint1, aPoint2).result()
assert (aBox8 is not None)
print "box8 : ok"

aSession.startOperation()
aPoint1 = model.addPoint(aDocument, 0, 0, 0).result()
aBox9 = model.addBox(aDocument, aPoint1, aPoint1)
assert (len(aBox9.results()) == 0)
print "box9 : ko (" + aBox9.feature().error() + ")"
aSession.finishOperation()

aSession.startOperation()
aPoint1 = model.addPoint(aDocument, 0, 0, 0).result()
aPoint2 = model.addPoint(aDocument, 0, 50, 50).result()
aBox10 = model.addBox(aDocument, aPoint1, aPoint2)
assert (len(aBox10.results()) == 0)
print "box10 : ko (" + aBox10.feature().error() + ")"
aSession.finishOperation()

aSession.startOperation()
aPoint1 = model.addPoint(aDocument, 0, 0, 0).result()
aPoint2 = model.addPoint(aDocument, 50, 0, 50).result()
aBox11 = model.addBox(aDocument, aPoint1, aPoint2)
assert (len(aBox11.results()) == 0)
print "box11 : ko (" + aBox11.feature().error() + ")"
aSession.finishOperation()

aSession.startOperation()
aPoint1 = model.addPoint(aDocument, 0, 0, 0).result()
aPoint2 = model.addPoint(aDocument, 50, 50, 0).result()
aBox12 = model.addBox(aDocument, aPoint1, aPoint2)
assert (len(aBox12.results()) == 0)
print "box12 : ko (" + aBox12.feature().error() + ")"
aSession.finishOperation()

aSession.startOperation()
aPoint1 = model.selection("VERTEX", "point1")
aPoint2 = model.addPoint(aDocument, 50, 50, 0).result()
aBox13 = model.addBox(aDocument, aPoint1, aPoint2)
assert (len(aBox13.results()) == 0)
print "box13 : ko (" + aBox13.feature().error() + ")"
aSession.finishOperation()

aSession.startOperation()
aPoint1 = model.addPoint(aDocument, 0, 0, 0).result()
aPoint2 = model.selection("VERTEX", "point2")
aBox14 = model.addBox(aDocument, aPoint1, aPoint2)
assert (len(aBox14.results()) == 0)
print "box14 : ko (" + aBox14.feature().error() + ")"
aSession.finishOperation()

aSession.startOperation()
Sketch_1 = model.addSketch(aDocument, model.defaultPlane("XOY"))
Sketch_2 = model.addSketch(aDocument, model.defaultPlane("YOZ"))
SketchLine_1 = Sketch_1.addLine(20, 10, 40, 10)
SketchLine_2 = Sketch_1.addLine(40, 10, 40, 20)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(40, 20, 20, 20)
SketchLine_4 = Sketch_1.addLine(20, 20, 20, 10)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
SketchLine_5 = Sketch_2.addLine(20, 10, 40, 10)
SketchLine_6 = Sketch_2.addLine(40, 10, 40, 20)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_2.addLine(40, 20, 20, 20)
SketchLine_8 = Sketch_2.addLine(20, 20, 20, 10)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_5.startPoint(), SketchLine_8.endPoint())
aSession.finishOperation()

aSession.startOperation()
aBox15 = model.addBox(aDocument, model.selection("VERTEX", "Sketch_1/Vertex-SketchLine_1s-SketchLine_4e"),
                      model.selection("VERTEX", "Sketch_2/Vertex-SketchLine_7s-SketchLine_6e"))
assert (aBox15 is not None)
print "box15 : ok"
aSession.finishOperation()

aSession.startOperation()
Extrusion_1 = model.addExtrusion(aDocument, [model.selection("COMPOUND", "Sketch_1")], model.selection("EDGE", "PartSet/OZ"), 100, 0)
Extrusion_2 = model.addExtrusion(aDocument, [model.selection("COMPOUND", "Sketch_2")], model.selection("EDGE", "PartSet/OX"), 100, 0)
aBox16 = model.addBox(aDocument, model.selection("VERTEX", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_2&Extrusion_1_1/To_Face_1"),
                       model.selection("VERTEX", "Extrusion_2_1/Generated_Face_2&Extrusion_2_1/Generated_Face_1&Extrusion_2_1/To_Face_1"))
assert (aBox16 is not None)
print "box16 : ok"
aSession.finishOperation()

aSession.startOperation()

Vertex_1 = model.addVertex(aDocument, [model.selection("VERTEX", "Sketch_1/Vertex-SketchLine_1s-SketchLine_4e")])
Vertex_2 = model.addVertex(aDocument, [model.selection("VERTEX", "Sketch_2/Vertex-SketchLine_7s-SketchLine_6e")])
aBox17 = model.addBox(aDocument, model.selection("VERTEX", "Vertex_1_1"), model.selection("VERTEX", "Vertex_2_1"))
assert (aBox17 is not None)
print "box17 : ok"
aSession.finishOperation()
