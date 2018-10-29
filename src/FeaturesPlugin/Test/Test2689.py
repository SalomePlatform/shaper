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

# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
model.addParameter(partSet, "hh", "20")
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-14.44396674123619, 36.663, -34.44396674123619, 36.663)
SketchLine_2 = Sketch_1.addLine(-34.44396674123619, 36.663, -34.44396674123619, 14.29964516129033)
SketchLine_3 = Sketch_1.addLine(-34.44396674123619, 14.29964516129033, -14.44396674123619, 14.29964516129033)
SketchLine_4 = Sketch_1.addLine(-14.44396674123619, 14.29964516129033, -14.44396674123619, 36.663)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(34.94274193548387, 39.45841935483872, 18.60029032258063, 39.45841935483872)
SketchLine_6 = Sketch_1.addLine(18.60029032258063, 39.45841935483872, 18.60029032258063, 13.00945161290323)
SketchLine_7 = Sketch_1.addLine(18.60029032258063, 13.00945161290323, 34.94274193548387, 13.00945161290323)
SketchLine_8 = Sketch_1.addLine(34.94274193548387, 13.00945161290323, 34.94274193548387, 39.45841935483872)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
SketchLine_9 = Sketch_1.addLine(-7.633645161290328, -5.05325806451613, -33.00745161290323, -5.05325806451613)
SketchLine_10 = Sketch_1.addLine(-33.00745161290323, -5.05325806451613, -33.00745161290323, -31.71725806451613)
SketchLine_11 = Sketch_1.addLine(-33.00745161290323, -31.71725806451613, -7.633645161290328, -31.71725806451613)
SketchLine_12 = Sketch_1.addLine(-7.633645161290328, -31.71725806451613, -7.633645161290328, -5.05325806451613)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_10.result())
SketchConstraintHorizontal_6 = Sketch_1.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_12.result())
SketchLine_13 = Sketch_1.addLine(36.44796774193549, -7.848677419354839, 19.46041935483872, -7.848677419354839)
SketchLine_14 = Sketch_1.addLine(19.46041935483872, -7.848677419354839, 19.46041935483872, -34.29764516129033)
SketchLine_15 = Sketch_1.addLine(19.46041935483872, -34.29764516129033, 36.44796774193549, -34.29764516129033)
SketchLine_16 = Sketch_1.addLine(36.44796774193549, -34.29764516129033, 36.44796774193549, -7.848677419354839)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintHorizontal_7 = Sketch_1.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_7 = Sketch_1.setVertical(SketchLine_14.result())
SketchConstraintHorizontal_8 = Sketch_1.setHorizontal(SketchLine_15.result())
SketchConstraintVertical_8 = Sketch_1.setVertical(SketchLine_16.result())
SketchCircle_1 = Sketch_1.addCircle(-24.6211935483871, 23.331, 4.515677419354841)
SketchCircle_2 = Sketch_1.addCircle(27.20158064516131, 20.96564516129033, 4.091267733479863)
SketchCircle_3 = Sketch_1.addCircle(-16.23493548387097, -18.60029032258065, 7.50151366286372)
SketchCircle_4 = Sketch_1.addCircle(27.84667741935485, -22.90093548387097, 5.414373911476542)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), "hh")
model.do()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "hh", "15")
model.addParameter(Part_1_doc, "hh_1", "26")
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "PartSet/Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchCircle_1_2r")], model.selection(), "hh", 0)
Extrusion_2_objects = [model.selection("WIRE", "PartSet/Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchCircle_1_2r_wire"), model.selection("FACE", "PartSet/Sketch_1/Face-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_8f-SketchCircle_2_2r"), model.selection("FACE", "PartSet/Sketch_1/Face-SketchLine_13f-SketchLine_14f-SketchLine_15f-SketchLine_16f-SketchCircle_4_2r"), model.selection("FACE", "PartSet/Sketch_1/Face-SketchLine_9f-SketchLine_10f-SketchLine_11f-SketchLine_12f-SketchCircle_3_2r")]
Extrusion_2 = model.addExtrusion(Part_1_doc, Extrusion_2_objects, model.selection(), 10, "hh_1")
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Extrusion_2_4"), model.selection("SOLID", "Extrusion_1_1")], model.selection("EDGE", "PartSet/OX"), 40)
Fuse_1_objects_1 = [model.selection("SOLID", "Translation_1_2"), model.selection("SOLID", "Translation_1_1"), model.selection("SOLID", "Extrusion_2_1")]
Fuse_1 = model.addFuse(Part_1_doc, Fuse_1_objects_1)
Translation_2 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Extrusion_2_3")], model.selection("EDGE", "PartSet/OX"), 30)
Common_1 = model.addCommon(Part_1_doc, [model.selection("SOLID", "Extrusion_2_2"), model.selection("SOLID", "Translation_2_1")])
model.do()
model.testHaveNamingSubshapes(Fuse_1, model, Part_1_doc)
model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.end()

assert(model.checkPythonDump())
