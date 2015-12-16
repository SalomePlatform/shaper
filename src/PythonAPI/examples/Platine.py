# Creation of Platine model using the end-user API
# Author: Sergey POKHODENKO
# -----------------------------

from GeomDataAPI import geomDataAPI_Point2D
import geom
import model

# Initialisation
model.begin()
partset = model.moduleDocument()

# Create a new Part
part = model.addPart(partset).document()

L = 64
E = 16
P = 80

# Create Parameters
model.addParameter(part, "L", str(L))
model.addParameter(part, "E", str(E))
model.addParameter(part, "P", str(P))

# Create YOZ sketch
sketch = model.addSketch(part, model.defaultPlane("YOZ"))

model.do()

# Create base polygon
H, L, r = 64, 64, 32
dx, dy = 0, 0

points = [(0, 0), (0, H), (L, H), (L, 0)]
points = [(p[0] + dx, p[1] + dy) for p in points]
geom_points = [geom.Pnt2d(*p) for p in points]
left, top, right, bottom = sketch.addPolygon(*geom_points)

model.do()

# Set constraints
sketch.setRigid(left.startPointData())

sketch.setHorizontal(bottom.result())
sketch.setHorizontal(top.result())

sketch.setVertical(right.result())
sketch.setVertical(left.result())

sketch.setLength(top.result(), "L")
sketch.setLength(left.result(), "L")

sketch.setFillet(left.endPointData(), r)

model.do()

# Create extrusion
YOZ_body = model.addExtrusion(part, sketch.selectFace(), "E")

model.do()

# Create XOY sketch
sketch = model.addSketch(part, model.defaultPlane("XOY"))

# Create base polygon
L, P = 64, P
dx, dy = E, 0

points = [(0, 0), (0, L), (P, L), (P, 16 + 16), (P - 20, 16 + 16), (P - 20, 16), (P, 16), (P, 0)]
points = [(p[0] + dx, p[1] + dy) for p in points]
geom_points = [geom.Pnt2d(*p) for p in points]
left, top, v2, h2, v1, h1, right, bottom = sketch.addPolygon(*geom_points)

points = [(P - 20, 16 + 16 / 2), (P - 20, 16), (P - 20, 16 + 16)]
points = [(p[0] + dx, p[1] + dy) for p in points]
center, start, end = [geom.Pnt2d(*p) for p in points]
arc = sketch.addArc(center, start, end)

model.do()

# Set Auxiliarity
v1.setAuxiliary(True)

# Set constraints
sketch.setCoincident(arc.center(), v1.result())
sketch.setCoincident(h2.endPointData(), arc.startPoint())
sketch.setCoincident(arc.endPoint(), h1.startPointData())

sketch.setPerpendicular(bottom.result(), right.result())
sketch.setPerpendicular(top.result(), left.result())
sketch.setPerpendicular(top.result(), v2.result())

sketch.setParallel(bottom.result(), h1.result())
sketch.setParallel(h1.result(), h2.result())
sketch.setParallel(h2.result(), top.result())
sketch.setParallel(v1.result(), v2.result())

sketch.setLength(top.result(), "P")
sketch.setLength(h2.result(), "20")
sketch.setLength(v1.result(), "16")
sketch.setLength(right.result(), "16")

model.do()

# Create extrusion
XOY_body = model.addExtrusion(part, sketch.selectFace(), "E")
# TODO(spo): collision with first object. Move object to the right place.

# Create XOZ sketch
plane = model.defaultPlane("XOZ")
plane.o.setY(64)
sketch = model.addSketch(part, plane)

# TODO(spo): select origin point

# Create base polygon
H, L, l, r = 28, 40, 8, 12
dx, dy = E, E

points = [(l + 2 * r, H), (L, H), (L, 0), (0, 0), (0, H), (l, H)]
points = [(p[0] + dx, p[1] + dy) for p in points]
geom_points = [geom.Pnt2d(*p) for p in points]
top_right, right, bottom, left, top_left = sketch.addPolyline(*geom_points)

points = [(l + r, H), (l, H), (l + 2 * r, H)]
points = [(p[0] + dx, p[1] + dy) for p in points]
center, start, end = [geom.Pnt2d(*p) for p in points]
arc = sketch.addArc(center, start, end)

model.do()

# Set constraints
sketch.setCoincident(top_left.endPointData(), arc.startPoint())
sketch.setCoincident(arc.endPoint(), top_right.endPointData())

sketch.setPerpendicular(bottom.result(), right.result())

sketch.setParallel(bottom.result(), top_left.result())
sketch.setParallel(bottom.result(), top_right.result())
sketch.setParallel(left.result(), right.result())

sketch.setLength(bottom.result(), L)
sketch.setLength(right.result(), H)
sketch.setLength(top_left.result(), l)

sketch.setRadius(arc.result(), r)

model.do()

# Create extrusion
XOZ_body = model.addExtrusion(part, sketch.selectFace(), "42")

model.do()

# Create XOZ 2nd sketch
plane = model.defaultPlane("XOZ")
plane.o.setY(64)
sketch = model.addSketch(part, plane)

# TODO(spo): make coincidence with existing points

# Create base polygon
dx, dy = E + L, E
H, L = 28, 96 - dx

points = [(0, 0), (0, H), (L, 0)]
points = [(p[0] + dx, p[1] + dy) for p in points]
geom_points = [geom.Pnt2d(*p) for p in points]
left, diagonal, bottom = sketch.addPolygon(*geom_points)

model.do()

# TODO(spo): make coincidence instead of constraints
sketch.setLength(bottom.result(), L)
sketch.setLength(left.result(), H)

sketch.setPerpendicular(bottom.result(), left.result())

model.do()

# Create extrusion
XOZ_2nd_body = model.addExtrusion(part, sketch.selectFace(), "12")

model.do()

# Create Boolean
boolean = model.addAddition(part, YOZ_body.result() + XOY_body.result() +
                                  XOZ_body.result() + XOZ_2nd_body.result(), [])

model.do()
