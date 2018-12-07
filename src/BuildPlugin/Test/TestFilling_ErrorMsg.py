## Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
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
Point_2 = model.addPoint(Part_1_doc, 10, 0, 0)
Point_3 = model.addPoint(Part_1_doc, 20, 10, 0)
Point_4 = model.addPoint(Part_1_doc, 0, 20, 10)
Point_5 = model.addPoint(Part_1_doc, 30, 0, 10)
Polyline_1 = model.addPolyline3D(Part_1_doc, [model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2")], False)
Polyline_2 = model.addPolyline3D(Part_1_doc, [model.selection("VERTEX", "Point_3"), model.selection("VERTEX", "Point_4")], False)
model.do()

Filling_1 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Polyline_1_1/Generated_Edge&Point_1/Point_1"), model.selection("EDGE", "Polyline_2_1/Generated_Edge&Point_3/Point_3")], "curve_info", 9, 8, 0, 0.0001, 0.0001, True)
assert(Filling_1.feature().error() != "")
Part_1_doc.removeFeature(Filling_1.feature())

Filling_1 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Polyline_1_1/Generated_Edge&Point_1/Point_1")])
assert(Filling_1.feature().error() != "")
Part_1_doc.removeFeature(Filling_1.feature())

Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

Filling_1 = model.addFilling(Part_1_doc, [model.selection("FACE", "Box_1_1/Top")])
assert(Filling_1.feature().error() != "")
Part_1_doc.removeFeature(Filling_1.feature())

model.end()

from ModelAPI import *
aSession = ModelAPI_Session.get()

aSession.startOperation()
aFillingFeature = featureToCompositeFeature(Part_1.feature()).addFeature("Filling")
aFillingFeature.string("advanced_options").setValue("")
aBaseObjectsList = aFillingFeature.selectionList("base_objects")
aBaseObjectsList.append("Polyline_1_1/Generated_Edge&Point_1/Point_1", "EDGE")
aBaseObjectsList.append("Box_1_1/Top", "FACE")
aFillingFeature.execute()
assert(aFillingFeature.error() != "")
aSession.finishOperation()
