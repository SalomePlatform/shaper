# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-87.19866468842729, 38.44668397626113, -52.3191988130564, 59.05727744807122)
SketchLine_2 = Sketch_1.addLine(-52.3191988130564, 59.05727744807122, -10.30529673590503, 36.86125370919882)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-10.30529673590503, 36.86125370919882, -59.45363501483681, 10.70165430267062)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-59.45363501483681, 10.70165430267062, -87.19866468842729, 38.44668397626113)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
SketchLine_5 = Sketch_1.addLine(-52.3191988130564, 59.05727744807122, -59.45363501483681, 10.70165430267062)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_5.endPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_6 = Sketch_2.addLine(-44.00943641618498, -5.404667630057807, -42.4652456647399, 37.06057803468208)
SketchLine_7 = Sketch_2.addLine(-42.4652456647399, 37.06057803468208, -11.58143063583817, 13.12562138728323)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_2.addLine(-11.58143063583817, 13.12562138728323, -16.2140028901734, -17.75819364161849)
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_2.addLine(-16.2140028901734, -17.75819364161849, -44.00943641618498, -5.404667630057807)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_6.startPoint(), SketchLine_9.endPoint())
SketchLine_10 = Sketch_2.addLine(-44.00943641618498, -5.404667630057807, -11.58143063583817, 13.12562138728323)
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_6.startPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_10.endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1"), model.selection("COMPOUND", "Sketch_2")], model.selection(), 10, 0)
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1"), model.selection("COMPSOLID", "Extrusion_1_2")], True)
model.testHaveNamingSubshapes(Fuse_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [1])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [11])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [54])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [108])

assert(model.checkPythonDump())
