from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Parameters
model.addParameter(Part_1_doc, "h", "90", "nut height")
model.addParameter(Part_1_doc, "a", "115.4")

# Step1. Sketch.
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-57.69999999999999, 99.93933159672423, 57.70000000000001, 99.93933159672423)
SketchLine_2 = Sketch_1.addLine(57.70000000000001, 99.93933159672423, 0, 0)
SketchLine_2.setAuxiliary(True)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(0, 0, -57.69999999999999, 99.93933159672423)
SketchLine_3.setAuxiliary(True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_4 = SketchProjection_2.createdFeature()
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_4.result(), SketchLine_1.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_1.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_2.result(), SketchLine_1.result())
SketchMultiRotation_1 = Sketch_1.addRotation([SketchLine_1.result()], SketchAPI_Line(SketchLine_4).startPoint(), 360, 6, True)
[SketchLine_5, SketchLine_6, SketchLine_7, SketchLine_8, SketchLine_9] = SketchMultiRotation_1.rotated()
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), "a")
model.do()

# Step2. Polyline.
Point_2 = model.addPoint(Part_1_doc, 50, 0, 0)
Point_2.result().setColor(0, 0, 255)
Point_3 = model.addPoint(Part_1_doc, 80, 0, 0)
Point_3.result().setColor(0, 0, 255)
Point_4 = model.addPoint(Part_1_doc, 112, 0, 13)
Point_4.result().setColor(0, 0, 255)
Point_5 = model.addPoint(Part_1_doc, 112, 0, 48)
Point_5.result().setColor(0, 0, 255)
Point_6 = model.addPoint(Part_1_doc, 80, 0, 63)
Point_6.result().setColor(0, 0, 255)
Point_7 = model.addPoint(Part_1_doc, "80", "0", "h")
Point_7.result().setColor(0, 0, 255)
Point_8 = model.addPoint(Part_1_doc, "50", "0", "h")
Point_8.result().setColor(0, 0, 255)
Polyline_1_objects = [model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2"),
                      model.selection("VERTEX", "Point_3"), model.selection("VERTEX", "Point_4"),
                      model.selection("VERTEX", "Point_5"), model.selection("VERTEX", "Point_6"),
                      model.selection("VERTEX", "Point_7")]
Polyline_1 = model.addPolyline3D(Part_1_doc, Polyline_1_objects, True)

# Step3. Face.
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Polyline_1_1")])

# Step4. Revolution.
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Face_1_1")], model.selection("EDGE", "PartSet/OZ"), 360, 0)

# Step5. Extrusion.
Wire = model.selection("WIRE", "Sketch_1/Face-SketchLine_5r-SketchLine_6r-SketchLine_7r-SketchLine_8r-SketchLine_9r-SketchLine_1r_wire")
To = model.selection("FACE", "Revolution_1_1/Generated_Face&Face_1_1/Edge_2")
From = model.selection("FACE", "Revolution_1_1/Generated_Face&Face_1_1/Edge_7")

Extrusion_1 = model.addExtrusion(Part_1_doc, [Wire], model.selection("EDGE", "PartSet/OZ"), To, 0, From, 0)

# Step6. Common.
Common_1 = model.addCommon(Part_1_doc, [model.selection("SOLID", "Revolution_1_1"), model.selection("SOLID", "Extrusion_1_1")])

# Step7. Fillet.
Fillet_1 = model.addFillet(Part_1_doc, [model.selection("EDGE", "[Revolution_1_1/Generated_Face&Face_1_1/Edge_3][Common_1_1/Modified_Face&Face_1_1/Edge_4]")], 10)

# Step8. Box.
Box_1 = model.addBox(Part_1_doc, 20, 120, 20)

# Step9. Translation.
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], -10, 20, 70)

# Step10. Angular Copy.
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Translation_1_1")], model.selection("EDGE", "PartSet/OZ"), 6)

# Step11. Cut.
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Fillet_1_1")], [model.selection("COMPOUND", "AngularCopy_1_1")])
Cut_1.result().setColor(255, 214, 0)

model.do()
model.end()
