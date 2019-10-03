from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchEllipse_1 = Sketch_1.addEllipse(-77.87951807228916, 4.624096385542177, -25.43227733721567, 4.190648115004366, 26.79776510151678)
[SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchLine_1, SketchLine_2] = SketchEllipse_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
SketchEllipse_2 = Sketch_1.addEllipse(86.88433734939758, -47.4578313253012, 121.4655255189626, -45.7605950961201, 19.46152401602575)
[SketchPoint_8, SketchPoint_9, SketchPoint_10, SketchPoint_11, SketchPoint_12, SketchPoint_13, SketchPoint_14, SketchLine_3, SketchLine_4] = SketchEllipse_2.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
model.do()
model.end()
