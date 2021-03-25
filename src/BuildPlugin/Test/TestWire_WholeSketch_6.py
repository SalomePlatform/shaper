# Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-20.13297844091623, -34.87132400696332, -10.14115693918956, -4.003004374441855)
SketchLine_2 = Sketch_1.addLine(-15.36017779982175, -20.12643132586926, -37.81263639928755, -17.52358908612689)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_1.result())
SketchLine_3 = Sketch_1.addLine(-37.81263639928755, -17.52358908612689, -28.23126689685475, -46.70782775211335)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-10.14115693918956, -4.003004374441855, -17.3289483797767, 22.37844327189705)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(-17.3289483797767, 22.37844327189705, -39.00238825664131, 1.575954908729718)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(-39.00238825664131, 1.575954908729718, -25.86766799285223, -18.90833134866903)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_2.result())
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], True)
model.end()

model.testNbResults(Wire_1, 3)
model.testNbSubShapes(Wire_1, GeomAPI_Shape.EDGE, [5, 1, 2])
model.testNbSubShapes(Wire_1, GeomAPI_Shape.VERTEX, [10, 2, 4])

assert(model.checkPythonDump())
