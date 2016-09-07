import model
import geom

model.begin()
partSet = model.moduleDocument()
part = model.addPart(partSet).document()

sketch = model.addSketch(part, model.defaultPlane("XOY"))
rectangle_1 = sketch.addRectangle(0, 0, 50, 50)

startPoint = geom.Pnt2d(50, 50)
endPoint = geom.Pnt2d(100, 100)

rectangle_2 = sketch.addRectangle(startPoint, endPoint)
model.end()

assert(model.checkPythonDump())
