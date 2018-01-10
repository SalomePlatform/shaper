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
param_d1 = model.addParameter(partSet, "d1", "5", "Chain roller diameter")
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "z", "25", "Number of teeth of the asterisk")
model.addParameter(Part_1_doc, "Lambda", "1.8", "Engagement chracteristic")
model.addParameter(Part_1_doc, "K", "0.565", "Tooth height coefficient")
model.addParameter(Part_1_doc, "De", "d1*Lambda*(K+1./tan(pi/z))", "External diameter")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 38.16365)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], "De/2")
model.end()

# due to the scenario, change the active document
model.begin()
from ModelAPI import *
aSession = ModelAPI_Session.get()
aSession.setActiveDocument(partSet, False)
model.do()
# change the parameter value
param_d1.setValue(8.51)
model.do()
# make the part document active back
aSession.setActiveDocument(Part_1_doc, False)
model.end()

# check the circle radius value
assert(SketchCircle_1.radius().value() == 64.9545)

assert(model.checkPythonDump())
