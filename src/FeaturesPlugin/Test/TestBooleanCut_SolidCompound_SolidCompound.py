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

# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-64.66755707084829, 12.13132840151158, 21.07964902130243)
SketchCircle_2 = Sketch_1.addCircle(45.61684376247555, 1.775642744728198, 22.96122819474142)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_3 = Sketch_2.addCircle(-36.49725872565889, 7.160803013521951, 17.872664608039)
SketchCircle_4 = Sketch_2.addCircle(78.03127953954764, -3.93858425080257, 22.76570299983515)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), 20, 20)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("SOLID", "Extrusion_1_2"), model.selection("SOLID", "Extrusion_1_1")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("SOLID", "Extrusion_2_2"), model.selection("SOLID", "Extrusion_2_1")])
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1")], [model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [2])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [8])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [48])

assert(model.checkPythonDump())
