from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchEllipse_1 = Sketch_1.addEllipse(-70, 5, -25, 10, -50, 40, True)
SketchEllipse_2 = Sketch_1.addEllipse(30, 50, 90, 10, 70, 50, False)
model.do()
model.end()
