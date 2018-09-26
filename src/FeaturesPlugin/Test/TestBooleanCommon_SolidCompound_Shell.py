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
SketchLine_1 = Sketch_1.addLine(-29.34869739478959, 31.37274549098196, -65.37675350701403, 31.37274549098196)
SketchLine_2 = Sketch_1.addLine(-65.37675350701403, 31.37274549098196, -65.37675350701403, -6.274549098196388)
SketchLine_3 = Sketch_1.addLine(-65.37675350701403, -6.274549098196388, -29.34869739478959, -6.274549098196388)
SketchLine_4 = Sketch_1.addLine(-29.34869739478959, -6.274549098196388, -29.34869739478959, 31.37274549098196)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(41.08817635270542, 32.5871743486974, 11.5370741482966, 32.5871743486974)
SketchLine_6 = Sketch_1.addLine(11.5370741482966, 32.5871743486974, 11.5370741482966, -8.703406813627252)
SketchLine_7 = Sketch_1.addLine(11.5370741482966, -8.703406813627252, 41.08817635270542, -8.703406813627252)
SketchLine_8 = Sketch_1.addLine(41.08817635270542, -8.703406813627252, 41.08817635270542, 32.5871743486974)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_9 = Sketch_2.addLine(-36.83851213815001, 4.871192068212162, -37.23534778672252, 23.01200273357838)
SketchLine_10 = Sketch_2.addLine(-37.23534778672252, 23.01200273357838, -8.095368404429529, 22.8820142517576)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchLine_11 = Sketch_2.addLine(-8.095368404429529, 22.8820142517576, 20.86605178109317, 22.08563377543123)
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchLine_12 = Sketch_2.addLine(20.86605178109317, 22.08563377543123, 22.14286343998464, -2.004860047909258)
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchLine_13 = Sketch_2.addLine(22.14286343998464, -2.004860047909258, -4.857114760245733, -1.198368777231491)
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchLine_14 = Sketch_2.addLine(-4.857114760245733, -1.198368777231491, -36.83851213815001, 4.871192068212162)
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_9.startPoint(), SketchLine_14.endPoint())
SketchLine_15 = Sketch_2.addLine(-4.857114760245733, -1.198368777231491, -8.095368404429529, 22.8820142517576)
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_15.endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 10)
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_9r-SketchLine_10r-SketchLine_14r-SketchLine_15f"), model.selection("FACE", "Sketch_2/Face-SketchLine_11r-SketchLine_12r-SketchLine_13r-SketchLine_15r")])
Compound_1 = model.addCompound(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_1_2")])
Common_1 = model.addCommon(Part_1_doc, [model.selection("SHELL", "Shell_1_1"), model.selection("COMPOUND", "Compound_1_1")])
model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [2])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [16])

assert(model.checkPythonDump())
