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
SketchCircle_1 = Sketch_1.addCircle(0.1822176134072968, -1.34047565209109, 35.08968954114807)
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchCircle_1_2")])
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], model.selection(), 100, 0)
Recover_1 = model.addRecover(Part_1_doc, Extrusion_1, [Wire_1.result()])
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Recover_1_1")])
Wire_2 = model.addWire(Part_1_doc, [model.selection("EDGE", "Extrusion_1_1/To_Edge")])
Face_2 = model.addFace(Part_1_doc, [model.selection("WIRE", "Wire_2_1")])
model.end()

from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Face_2.feature()))

assert(model.checkPythonDump())
