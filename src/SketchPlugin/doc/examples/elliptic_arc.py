from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchEllipticArc_1 = Sketch_1.addEllipticArc(-130.9349397590362, 14.84578313253013, -130.1669839639096, 36.73252329363673, -113.8987951807229, 13.87228915662651, -144.7734498958671, 30.30133508641346, False)
[SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchLine_1, SketchLine_2] = SketchEllipticArc_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
model.do()
model.end()
