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
SketchCircle_1 = Sketch_1.addCircle(-38.80617495711836, 0.1732418524871273, 42.63017006028262)
SketchCircle_2 = Sketch_1.addCircle(-117.8044596912521, -0.1732418524871385, 54.50817511994374)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Remove_SubShapes_1 = model.addRemoveSubShapes(Part_1_doc, model.selection("COMPSOLID", "Extrusion_1_1"))
Remove_SubShapes_1.setSubShapesToRemove([model.selection("SOLID", "Extrusion_1_1_3")])
model.do()
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Remove_SubShapes_1, 1)
model.testNbSubResults(Remove_SubShapes_1, [2])
model.testNbSubShapes(Remove_SubShapes_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Remove_SubShapes_1, GeomAPI_Shape.FACE, [10])
model.testNbSubShapes(Remove_SubShapes_1, GeomAPI_Shape.EDGE, [36])
model.testNbSubShapes(Remove_SubShapes_1, GeomAPI_Shape.VERTEX, [72])
model.testResultsVolumes(Remove_SubShapes_1, [136619.795923917088657617568969727])

assert(model.checkPythonDump())
