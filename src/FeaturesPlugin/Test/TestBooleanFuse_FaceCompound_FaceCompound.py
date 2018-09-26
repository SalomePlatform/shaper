## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-35.3684929577465, 31.98214788732393, 24.26807713993482)
SketchCircle_2 = Sketch_1.addCircle(51.92395774647893, 31.22962676056337, 25.62994603225585)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_3 = Sketch_2.addCircle(-36.12101408450705, -7.148950704225357, 25.24032590912034)
SketchCircle_4 = Sketch_2.addCircle(51.92395774647893, -12.41659859154929, 31.3630728131964)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f")])
Face_2 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f")])
Face_3 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_3_2f")])
Face_4 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_4_2f")])
Compound_1 = model.addCompound(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("FACE", "Face_2_1")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("FACE", "Face_3_1"), model.selection("FACE", "Face_4_1")])
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1"), model.selection("COMPOUND", "Compound_2_1")], True)
model.testHaveNamingSubshapes(Fuse_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [2])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [8])

assert(model.checkPythonDump())
