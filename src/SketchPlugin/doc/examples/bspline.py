from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))

SketchBSpline_1_poles = [(-70, -5), (-50, 30), (-40, 3), (-20, 20), (-10, -5)]
SketchBSpline_1 = Sketch_1.addSpline(poles = SketchBSpline_1_poles, weights = [1, 2, 2, 2, 1])
[SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5] = SketchBSpline_1.controlPoles(auxiliary = [0, 1, 2, 3, 4])
[SketchLine_1, SketchLine_2, SketchLine_3, SketchLine_4] = SketchBSpline_1.controlPolygon(auxiliary = [0, 1, 2, 3])

SketchBSplinePeriodic_1_poles = [(10, -5), (30, 30), (40, 3), (60, 20), (70, -5)]
SketchBSplinePeriodic_1 = Sketch_1.addSpline(poles = SketchBSplinePeriodic_1_poles, weights = [3, 2, 1, 1, 1], periodic = True)
[SketchPoint_6, SketchPoint_7, SketchPoint_8, SketchPoint_9, SketchPoint_10] = SketchBSplinePeriodic_1.controlPoles(auxiliary = [0, 1, 2, 3, 4])
[SketchLine_5, SketchLine_6, SketchLine_7, SketchLine_8, SketchLine_9] = SketchBSplinePeriodic_1.controlPolygon(auxiliary = [0, 1, 2, 3, 4])

model.do()
model.end()
