# -*- coding: utf-8 -*-

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Step1. Sketch.
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-57.7, 99.93933159672423, 57.7, 99.93933159672423)
SketchLine_2 = Sketch_1.addLine(57.7, 99.93933159672423, 0, 0)
SketchLine_2.setAuxiliary(True)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(0, 0, -57.7, 99.93933159672423)
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
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 115.4)
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
Point_7 = model.addPoint(Part_1_doc, 80, 0, 90)
Point_7.result().setColor(0, 0, 255)
Point_8 = model.addPoint(Part_1_doc, 50, 0, 90)
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
Wire_Name = "Sketch_1/Wire-SketchLine_1r-SketchLine_5r-SketchLine_6r-SketchLine_7r-SketchLine_8r-SketchLine_9r"
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", Wire_Name)], model.selection("EDGE", "PartSet/OZ"), 100, 0)

# Step6. Common.
Common_1 = model.addCommon(Part_1_doc, [model.selection("SOLID", "Revolution_1_1"), model.selection("SOLID", "Extrusion_1_1")])

# Step7. Fillet.
Fillet_1 = model.addFillet(Part_1_doc, [model.selection("EDGE", "Revolution_1_1/Generated_Face_5&Common_1_1/Modified_Face_10")], 10)

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
