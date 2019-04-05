from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-56.65008576329333, 13.33962264150944, 9.528301886792452, 66.69811320754719)
SketchLine_2 = Sketch_1.addLine(9.528301886792452, 66.69811320754719, 78.47855917667239, -27.54545454545455)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(78.47855917667239, -27.54545454545455, 7.795883361921087, -89.2195540308748)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(7.795883361921087, -89.2195540308748, 12.64665523156088, -20.9622641509434)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(12.64665523156088, -20.9622641509434, -56.65008576329333, 13.33962264150944)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_1.startPoint())
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_5r-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")])
SubShapes_1 = model.addSubShapes(Part_1_doc, model.selection("FACE", "Face_1_1"), [model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_5")])
model.do()
model.end()
