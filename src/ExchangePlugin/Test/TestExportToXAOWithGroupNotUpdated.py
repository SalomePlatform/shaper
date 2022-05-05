# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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

# This test checks that exportToXAO works even if a group or a field is not valid

from salome.shaper import model
from ModelAPI import featureToCompositeFeature
from GeomAPI import GeomAPI_Shape
import tempfile
import os

def getTmpFileName(ext):
  tempdir = tempfile.gettempdir()
  tmp_file = tempfile.NamedTemporaryFile(suffix=".%s"%ext, prefix='shaper_', dir=tempdir, delete=False)
  tmp_filename = tmp_file.name
  return tmp_filename

tmp_file1 = getTmpFileName("xao")
tmp_file2 = getTmpFileName("xao")

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
# Create a box
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 0, 0, 50)
SketchLine_2 = Sketch_1.addLine(0, 50, 50, 50)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(50, 50, 50, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(50, 0, 0, 0)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintRigid_1 = Sketch_1.setFixed(SketchLine_1.startPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_4.result(), 50)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_1.result(), 50)
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_1.result(), SketchLine_3.result())
SketchConstraintParallel_2 = Sketch_1.setParallel(SketchLine_2.result(), SketchLine_4.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_1.result(), SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 50, 0)
# Create a group with the faces of the box
Group_1_objects = [model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1"), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2"), model.selection("FACE", "Extrusion_1_1/From_Face"), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4"), model.selection("FACE", "Extrusion_1_1/To_Face"), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_1.setName("Group_faces")
Group_1.result().setName("Group_faces")
# Create a field on the box
Field_1_objects = [model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1"), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2"), model.selection("FACE", "Extrusion_1_1/From_Face"), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4"), model.selection("FACE", "Extrusion_1_1/To_Face"), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3")]
Field_1 = model.addField(Part_1_doc, 1, "DOUBLE", 1, ["Comp 1"], Field_1_objects)
Field_1.addStep(0, 0, [[0], [1], [2], [3], [4], [5], [6]])
# Create a plane to split the box in two parts
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), 25, False)
# Split the Box in two parts
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("FACE", "Plane_1")])
# Export the shape with the group and field not updated with the new result
Export_1 = model.exportToXAO(Part_1_doc, tmp_file1)
# Update the group and the field with the last result
Part_1_doc.moveFeature(Group_1.feature(), Partition_1.feature())
Part_1_doc.moveFeature(Field_1.feature(), Group_1.feature())
# Export the shape with the group and field updated
Export_2 = model.exportToXAO(Part_1_doc, tmp_file2)
model.do()

# Create a new part
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
# Import the first XAO file
Import_1 = model.addImport(Part_2_doc, tmp_file1)
model.do()

# Check results
Import_1_Feature = Import_1.feature()
assert Import_1_Feature.error() == ''
assert Import_1_Feature.name() == "Partition_1_1"
assert len(Import_1_Feature.results()) == 1
model.testNbSubShapes(Import_1, GeomAPI_Shape.SOLID, [2])
# No group, no field
aCompositeFeature = featureToCompositeFeature(Import_1_Feature)
assert aCompositeFeature.numberOfSubs(False) == 0

# Create a new part
Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()

# Import the second XAO file
Import_2 = model.addImport(Part_3_doc, tmp_file2)
model.do()

# Check results
Import_2_Feature = Import_2.feature()
assert Import_2_Feature.error() == ''
assert Import_2_Feature.name() == "Partition_1_1"
assert len(Import_2_Feature.results()) == 1
model.testNbSubShapes(Import_2, GeomAPI_Shape.SOLID, [2])

# One group, one field
aCompositeFeature = featureToCompositeFeature(Import_2_Feature)
assert aCompositeFeature.numberOfSubs(False) == 2

aFeature1 = aCompositeFeature.subFeature(0, False)
assert aFeature1.getKind() == "Group"
assert aFeature1.name() == "Group_faces"

aSelectionList = aFeature1.selectionList("group_list")
assert aSelectionList.selectionType() == "face"
assert aSelectionList.size() == 10

aFeature2 = aCompositeFeature.subFeature(1, False)
assert aFeature2.getKind() == "Field"
assert aFeature2.name() == "Field_1"
assert aFeature2.intArray("stamps").size() == 1
assert aFeature2.tables("values").rows() == 7
assert aFeature2.tables("values").columns() == 1
assert aFeature2.tables("values").tables() == 1
assert aFeature2.tables("values").type() == 2 # double
assert aFeature2.selectionList("selected").size() == 6
model.do()
model.end()

os.remove(tmp_file1)
os.remove(tmp_file2)
