## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)

Smash_1 = model.addSmash(Part_1_doc, [model.selection("FACE", "Cylinder_1_1/Face_1")], [model.selection("SOLID", "Cylinder_1_1")])
assert(Smash_1.feature().error() != "")
Part_1_doc.removeFeature(Smash_1.feature())

Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Cylinder_1_1/Face_1")])

Smash_1 = model.addSmash(Part_1_doc, [model.selection("FACE", "Face_1_1")], [model.selection("SOLID", "Cylinder_1_1")])
assert(Smash_1.feature().error() != "")
Part_1_doc.removeFeature(Smash_1.feature())

Smash_1 = model.addSmash(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")], [model.selection("FACE", "Face_1_1")])
assert(Smash_1.feature().error() != "")
Part_1_doc.removeFeature(Smash_1.feature())

Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(5, -6, 7)
SketchCircle_2 = Sketch_1.addCircle(14, 0, 8)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)

Smash_1 = model.addSmash(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1_1")], [model.selection("SOLID", "Extrusion_1_1_2")])
assert(Smash_1.feature().error() != "")
Part_1_doc.removeFeature(Smash_1.feature())

Face_2 = model.addFace(Part_1_doc, [model.selection("FACE", "Extrusion_1_1_1/From_Face"), model.selection("FACE", "Extrusion_1_1_1/To_Face")])

Smash_1 = model.addSmash(Part_1_doc, [model.selection("FACE", "Face_2_1")], [model.selection("FACE", "Face_2_2")])
assert(Smash_1.feature().error() != "")
Part_1_doc.removeFeature(Smash_1.feature())

model.end()


from ModelAPI import *
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

aSession.startOperation()
Smash_1 = Part_1_doc.addFeature("Smash")
Smash_1.execute()
assert(Smash_1.error() != "")
aSession.finishOperation()
