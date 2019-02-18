# Copyright (C) 2018-2019  CEA/DEN, EDF R&D
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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Point_2 = model.addPoint(Part_1_doc, 0, 0, 0)
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_1")])

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("VERTEX", "Vertex_1_1")], model.selection(), 10, 0)
assert(Extrusion_1.feature().error() != "")
Part_1_doc.removeFeature(Extrusion_1.feature())

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(20, 20, 10)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection("EDGE", "PartSet/OX"), 10, 0)
assert(Extrusion_1.feature().error() != "")
Part_1_doc.removeFeature(Extrusion_1.feature())

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection("EDGE", "PartSet/OX"), 10, 0)
assert(Extrusion_1.feature().error() != "")
Part_1_doc.removeFeature(Extrusion_1.feature())

Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchArc_1 = Sketch_2.addArc(25, 10, 40, 10, 25, 25, False)
model.do()

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection("EDGE", "Sketch_2/SketchArc_1_2"), 10, 0)
assert(Extrusion_1.feature().error() != "")
Part_1_doc.removeFeature(Extrusion_1.feature())

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), model.selection("EDGE", "Sketch_2/SketchArc_1_2"), 0, model.selection("EDGE", "Sketch_2/SketchArc_1_2"), 10)
assert(Extrusion_1.feature().error() != "")
Part_1_doc.removeFeature(Extrusion_1.feature())

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), model.selection("EDGE", "Sketch_2/SketchArc_1_2"), 0, model.selection("FACE", "PartSet/XOY"), 10)
assert(Extrusion_1.feature().error() != "")
Part_1_doc.removeFeature(Extrusion_1.feature())

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), model.selection("EDGE", "Sketch_2/SketchArc_1_2"), 0, model.selection(), 10)
assert(Extrusion_1.feature().error() != "")
Part_1_doc.removeFeature(Extrusion_1.feature())

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), model.selection(), 0, model.selection("EDGE", "Sketch_2/SketchArc_1_2"), 10)
assert(Extrusion_1.feature().error() != "")
Part_1_doc.removeFeature(Extrusion_1.feature())

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), model.selection(), 0, model.selection("FACE", "PartSet/XOY"), 10)
assert(Extrusion_1.feature().error() == "")
#Part_1_doc.removeFeature(Extrusion_1.feature())

model.end()
