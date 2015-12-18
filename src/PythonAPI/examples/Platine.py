# Creation of Platine model using the end-user API
# Author: Sergey POKHODENKO
# -----------------------------

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
model.addParameter(part, "L", L)
model.addParameter(part, "E", E)
model.addParameter(part, "P", P)

def vertical_body():
    # Create YOZ sketch
    sketch = model.addSketch(part, model.defaultPlane("YOZ"))

    points = [(0, 0), (0, L), (L, L), (L, 0)]
    geom_points = [geom.Pnt2d(*p) for p in points]
    left, top, right, bottom = sketch.addPolygon(*geom_points)

    # Set constraints
    sketch.setRigid(left.startPoint())

    sketch.setHorizontal(bottom.result())
    sketch.setHorizontal(top.result())

    sketch.setVertical(right.result())
    sketch.setVertical(left.result())

    sketch.setLength(top.result(), "L")
    sketch.setLength(left.result(), "L")

    sketch.setFillet(left.endPoint(), 32)

    model.do()  #!!!

    # Create extrusion
    body = model.addExtrusion(part, sketch.selectFace(), "E")

    model.do()

    return body

def bottom_body():
    # Create XOY sketch
    sketch = model.addSketch(part, "Extrusion_1_1/LateralFace_2")

    # Create base polygon
    points = [(0, 0), (0, L), (P, L), (P, 16 + 16), (P - 20, 16 + 16), (P - 20, 16), (P, 16), (P, 0)]
    points = [(p[0], -p[1]) for p in points]  # as we look to back of the face
    geom_points = [geom.Pnt2d(*p) for p in points]
    left, top, v2, h2, v1, h1, right, bottom = sketch.addPolygon(*geom_points)

    points = [(P - 20, 16 + 16 / 2), (P - 20, 16), (P - 20, 16 + 16)]
    points = [(p[0], -p[1]) for p in points]  # as we look to back of the face
    center, start, end = [geom.Pnt2d(*p) for p in points]
    arc = sketch.addArc(center, start, end, inversed=True)

    # Set Auxiliarity
    v1.setAuxiliary(True)

    # Set constraints
    sketch.setParallel(left.result(), right.result())
    sketch.setParallel(left.result(), v2.result())
    sketch.setParallel(bottom.result(), h1.result())
    sketch.setParallel(top.result(), h2.result())

    sketch.setPerpendicular(left.result(), bottom.result())
    sketch.setPerpendicular(left.result(), top.result())

    sketch.setEqual(top.result(), bottom.result())
    sketch.setEqual(h1.result(), h2.result())

    sketch.setLength(top.result(), "P")
    sketch.setLength(right.result(), 16)
    sketch.setLength(v1.result(), 16)
    sketch.setLength(h2.result(), 20)

    sketch.setCoincident(arc.center(), v1.result())
    sketch.setCoincident(arc.startPoint(), h2.endPoint())
    sketch.setCoincident(arc.endPoint(), h1.startPoint())

    # Binding
    left_e = sketch.addLine("Extrusion_1_1/LateralFace_2|Extrusion_1_1/ToFace_1")
    sketch.setCoincident(left_e.startPoint(), left.endPoint())
    sketch.setCoincident(left_e.endPoint(), left.startPoint())

    model.do()  #!!!

    # Create extrusion
    body = model.addExtrusion(part, sketch.selectFace(), "-E")

    model.do()

    return body

def body_3():
    # Create XOZ sketch
    sketch = model.addSketch(part, "Boolean_1_1/Modified_3")

    # Create base polygon
    H, L, l, r = 28, 40, 8, 12

    points = [(0, 0), (0, H), (l, H), (l + 2 * r, H), (L, H), (L, 0)]
    points = [(p[0], -p[1]) for p in points]  # as we look to back of the face
    geom_points = [geom.Pnt2d(*p) for p in points]
    left, top_left, top_middle, top_right, right, bottom, = sketch.addPolygon(*geom_points)

    points = [(l + r, H), (l, H), (l + 2 * r, H)]
    points = [(p[0], -p[1]) for p in points]  # as we look to back of the face
    center, start, end = [geom.Pnt2d(*p) for p in points]
    arc = sketch.addArc(center, start, end)

    # Set Auxiliarity
    top_middle.setAuxiliary(True)

    # Set constraints
    sketch.setParallel(bottom.result(), top_left.result())
    sketch.setParallel(bottom.result(), top_right.result())

    sketch.setPerpendicular(bottom.result(), left.result())
    sketch.setPerpendicular(bottom.result(), right.result())

    sketch.setEqual(left.result(), right.result())

    sketch.setLength(bottom.result(), L)
    sketch.setLength(right.result(), H)
    sketch.setLength(top_left.result(), l)

    sketch.setCoincident(top_middle.result(), arc.center())
    sketch.setCoincident(top_middle.endPoint(), arc.startPoint())
    sketch.setCoincident(top_middle.startPoint(), arc.endPoint())

    sketch.setRadius(arc.result(), r)

    # Binding
    bottom_e = sketch.addLine("Boolean_1_1/Modified_1|Boolean_1_1/Modified_3")
    sketch.setCoincident(bottom_e.result(), bottom.startPoint())
    sketch.setCoincident(bottom_e.startPoint(), bottom.endPoint())

    model.do()  #!!!

    # Create extrusion
    body = model.addExtrusion(part, sketch.selectFace(), "-(L-22)")

    model.do()  #!!!

    return body

def body_4():
    # Create XOZ 2nd sketch
    sketch = model.addSketch(part, "Boolean_2_1/Modified_7")

    # Create base polygon
    points = [(0, 0), (0, 1), (1, 0)]
    points = [(p[0], -p[1]) for p in points]  # as we look to back of the face
    geom_points = [geom.Pnt2d(*p) for p in points]
    left, diagonal, bottom = sketch.addPolygon(*geom_points)

    # Binding
    bottom_e = sketch.addLine("Boolean_2_1/Modified_8|Boolean_2_1/Modified_7")
    sketch.setCoincident(bottom_e.endPoint(), bottom.startPoint())
    sketch.setCoincident(bottom_e.startPoint(), left.startPoint())

    left_e = sketch.addLine("Boolean_2_1/Modified_3|Boolean_2_1/Modified_2")
    sketch.setCoincident(left_e.startPoint(), left.endPoint())

    model.do()  #!!!

    # Create extrusion
    body = model.addExtrusion(part, sketch.selectFace(), "-12")

    model.do()  #!!!

    return body


b1 = vertical_body()
b2 = bottom_body()

boolean = model.addAddition(part, b1.result() + b2.result(), [])
model.do()

b3 = body_3()

boolean = model.addAddition(part, boolean.result() + b3.result(), [])
model.do()

b4 = body_4()

boolean = model.addAddition(part, boolean.result() + b4.result(), [])
model.do()
