# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# Creation of Platine model using the end-user API
# Author: Sergey POKHODENKO
# -----------------------------

from salome.shaper import geom
from salome.shaper import model

# START DEBUG PURPOSES
# Should be removed
import os
import ModelAPI
# END DEBUG PURPOSES

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

def vertical_body():
    # Create YOZ sketch
    sketch = model.addSketch(part, model.defaultPlane("YOZ"))

    points = [(0, 0), (0, L), (L, L), (L, 0)]
    geom_points = [geom.Pnt2d(*p) for p in points]
    left, top, right, bottom = model.addPolygon(sketch, *geom_points)

    # Set constraints
    sketch.setFixed(left.startPoint())

    sketch.setHorizontal(bottom)
    sketch.setHorizontal(top)

    sketch.setVertical(right)
    sketch.setVertical(left)

    sketch.setLength(top, "L")
    sketch.setLength(left, "L")

    sketch.setFilletWithRadius(left.endPoint(), 32)

    model.do()  #!!!

    # Create extrusion
    body = model.addExtrusion(part, sketch.selectFace(), "E")

    return body

def bottom_body():
    # Create XOY sketch
    sketch = model.addSketch(part, "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4")

    # Create base polygon
    points = [(0, 0), (0, L), (P, L), (P, 16 + 16), (P - 20, 16 + 16), (P - 20, 16), (P, 16), (P, 0)]
    points = [(p[0], -p[1]) for p in points]  # as we look to back of the face
    geom_points = [geom.Pnt2d(*p) for p in points]
    left, top, v2, h2, v1, h1, right, bottom = model.addPolygon(sketch, *geom_points)

    points = [(P - 20, 16 + 16 / 2), (P - 20, 16 + 16), (P - 20, 16)]
    points = [(p[0], -p[1]) for p in points]  # as we look to back of the face
    center, start, end = [geom.Pnt2d(*p) for p in points]
    arc = sketch.addArc(center, start, end, True)

    # Set Auxiliarity
    v1.setAuxiliary(True)

    # Set constraints
    sketch.setParallel(left, right)
    sketch.setParallel(left, v2)
    sketch.setParallel(bottom, h1)
    sketch.setParallel(top, h2)

    sketch.setPerpendicular(left, bottom)
    sketch.setPerpendicular(left, top)

    sketch.setEqual(top, bottom)
    sketch.setEqual(h1, h2)

    sketch.setCoincident(arc.center(), v1)
    sketch.setCoincident(arc.startPoint(), h2.endPoint())
    sketch.setCoincident(arc.endPoint(), h1.startPoint())

    # Binding
    left_e = sketch.addLine("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1/To_Face]")
    sketch.setCoincident(left_e.startPoint(), left.endPoint())
    sketch.setCoincident(left_e.endPoint(), left.startPoint())

    # Dimensions
    #sketch.setLength(v1, 16)
    sketch.setLength(v2, 32)
    sketch.setLength(h2, 20)
    sketch.setLength(right, 16)
    sketch.setLength(top, "P")
    model.do()

    # Create extrusion
    body = model.addExtrusion(part, sketch.selectFace(), "-E")

    model.do()

    return body

def body_3():
    # Create XOZ sketch
    sketch = model.addSketch(part, "Boolean_1_1/Modified_Face&Sketch_1/SketchLine_3")

    # Create base polygon
    H, L, l, r = 28, 40, 8, 12

    points = [(0, 0), (0, H), (l, H), (l + 2 * r, H), (L, H), (L, 0)]
    points = [(p[0], -p[1]) for p in points]  # as we look to back of the face
    geom_points = [geom.Pnt2d(*p) for p in points]
    left, top_left, top_middle, top_right, right, bottom, = model.addPolygon(sketch, *geom_points)

    points = [(l + r, H), (l + 2 * r, H), (l, H)]
    points = [(p[0], -p[1]) for p in points]  # as we look to back of the face
    center, start, end = [geom.Pnt2d(*p) for p in points]
    arc = sketch.addArc(center, start, end, False)

    # Set Auxiliarity
    top_middle.setAuxiliary(True)

    # Set constraints
    sketch.setParallel(bottom, top_left)
    sketch.setParallel(bottom, top_right)

    sketch.setPerpendicular(bottom, left)
    sketch.setPerpendicular(bottom, right)

    sketch.setEqual(left, right)

    sketch.setLength(bottom, L)
    sketch.setLength(right, H)
    sketch.setLength(top_left, l)

    sketch.setCoincident(top_middle, arc.center())
    sketch.setCoincident(top_middle.endPoint(), arc.startPoint())
    sketch.setCoincident(top_middle.startPoint(), arc.endPoint())

    sketch.setRadius(arc, r)

    # Binding
    bottom_e = sketch.addLine("[Boolean_1_1/Modified_Face&Sketch_2/SketchLine_6][Extrusion_2_1/To_Face]")
    sketch.setCoincident(bottom_e, bottom.startPoint())
    sketch.setCoincident(bottom_e.startPoint(), bottom.endPoint())

    model.do()  #!!!

    # Create extrusion
    body = model.addExtrusion(part, sketch.selectFace(), "-(L-22)")

    return body

def body_4():
    # Create XOZ 2nd sketch
    sketch = model.addSketch(part, "Boolean_2_1/Modified_Face&Sketch_2/SketchLine_6")

    # Create base polygon
    points = [(0, 0), (0, 1), (1, 0)]
    points = [(p[0], -p[1]) for p in points]  # as we look to back of the face
    geom_points = [geom.Pnt2d(*p) for p in points]
    left, diagonal, bottom = model.addPolygon(sketch, *geom_points)

    # Binding
    bottom_e = sketch.addLine("[Boolean_2_1/Modified_Face&Extrusion_2_1/To_Face][Boolean_2_1/Modified_Face&Sketch_2/SketchLine_6]")
    sketch.setCoincident(bottom_e.endPoint(), bottom.startPoint())
    sketch.setCoincident(bottom_e.startPoint(), left.startPoint())

    left_e = sketch.addLine("[Boolean_2_1/Modified_Face&Extrusion_3_1/From_Face][Extrusion_3_1/Generated_Face&Sketch_3/SketchLine_18]")
    sketch.setCoincident(left_e.startPoint(), left.endPoint())

    model.do()  #!!!

    # Create extrusion
    body = model.addExtrusion(part, sketch.selectFace(), "-12")

    return body


b1 = vertical_body()
b2 = bottom_body()

boolean = model.addFuse(part, b1.results() + b2.results())
boolean.result().setName("Boolean_1_1")
model.do()

b3 = body_3()

boolean = model.addFuse(part, boolean.results() + b3.results())
boolean.result().setName("Boolean_2_1")
model.do()

# START DEBUG PURPOSES
# prepare a study without last operation to trap floating problem with degenerated line
#aPathToStore = os.path.join(os.getcwd(), "Data")
#print aPathToStore
#if not os.path.exists(aPathToStore):
#    os.mkdir(aPathToStore)
#results = ModelAPI.StringList()
#ModelAPI.ModelAPI_Session.get().save(aPathToStore, results)
# END DEBUG PURPOSES
b4 = body_4()

boolean = model.addFuse(part, boolean.results() + b4.results())
model.end()

assert(model.checkPythonDump())
