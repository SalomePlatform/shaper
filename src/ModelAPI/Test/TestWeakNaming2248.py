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
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(162.2066367177775, 151.0353807454788, -151.0353807454788, 151.0353807454788)
SketchLine_2 = Sketch_1.addLine(-151.0353807454788, 151.0353807454788, -151.0353807454788, -54.96257938370976)
SketchLine_3 = Sketch_1.addLine(-151.0353807454788, -54.96257938370976, 162.2066367177775, -54.96257938370976)
SketchLine_4 = Sketch_1.addLine(162.2066367177775, -54.96257938370976, 162.2066367177775, 151.0353807454788)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchArc_1 = Sketch_1.addArc(-6.962011033302339, 151.0353807454788, -151.0353807454788, 151.0353807454788, 38.42912054470763, 14.29920764454234, False)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchArc_1.startPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_2.result(), SketchArc_1.results()[1])
SketchArc_2 = Sketch_1.addArc(162.2066367177775, 234.3997613131608, 162.2066367177775, -54.96257938370976, -46.91927508365465, 34.40746839468006, True)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_2.startPoint())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchLine_3.result(), SketchArc_2.results()[1])
SketchCircle_1 = Sketch_1.addCircle(-8.043304300055087, 108.5846080507436, 59.51166177475501)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
model.do()
model.testHaveNamingSubshapes(Extrusion_1, model, Part_1_doc)
model.end()
