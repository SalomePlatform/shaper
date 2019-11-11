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

from ConfigAPI import *
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 12.99991029434202, 0, 51.37466648339316)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(0, 51.37466648339316, 51.95275254614653, 51.37466648339316)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(51.95275254614653, 51.37466648339316, 59.69545386375759, 37.96391441346034)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(59.69545386375759, 37.96391441346034, 133.2087031398387, 37.96391441346034)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(133.2087031398387, 37.96391441346034, 133.2087031398387, 12.99991029434202)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(133.2087031398387, 12.99991029434202, 0, 12.99991029434202)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_7.endPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintAngle_1 = Sketch_1.setAngleBackward(SketchLine_4.result(), SketchLine_2.result(), 30)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_2.result())
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_7r-SketchLine_6r-SketchLine_5r-SketchLine_4r-SketchLine_3r-SketchLine_1r")], model.selection("EDGE", "PartSet/OX"), 360, 0)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_6"), 1, True)
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/YOZ"), 1, False)

# load 'Filters' plugin
FiltersFeature = model.filters(Part_1_doc, [model.addFilter(name = "BelongsTo", args = [model.selection("FACE", "Plane_1")])])

FiltersFactory = ModelAPI_Session.get().filters()
Filter = FiltersFactory.filter("BelongsTo")

# read XML representation to load validator
ValidatorReader = Config_ValidatorReader(Filter.xmlRepresentation(), True)
ValidatorReader.setFeatureId("FiltersSelection")
ValidatorReader.readAll()

aFactory = ModelAPI_Session.get().validators()
# no Group, thus not valid yet
assert(not aFactory.validate(FiltersFeature.feature()))

# should be not valid while the group is created
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Box_1_1/Left"), FiltersFeature])
assert(not aFactory.validate(FiltersFeature.feature()))

model.end()
