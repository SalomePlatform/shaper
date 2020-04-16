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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-23.21770404974615, 0.697956782029321, 35.59095357943583)
SketchLine_1 = Sketch_1.addLine(-23.86759161364122, 36.28297642299413, -22.5735889911581, -34.88716781357743)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchCircle_1.results()[1])
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_2 = Sketch_2.addCircle(32.08143103297851, 1.286314091165838, 40.2404383217231)
SketchLine_2 = Sketch_2.addLine(-8.153954725266397, 0.6486549270159857, 72.23884334545455, 3.869938036363647)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_2.startPoint(), SketchCircle_2.results()[1])
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchCircle_2.results()[1])
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), 20, 20)
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1")], [model.selection("COMPSOLID", "Extrusion_2_1")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [2])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [11])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [42])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [84])

assert(model.checkPythonDump())
