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

import os
import sys
import salome

salome.salome_init()

#=========================================================================
# A function to get the XAO file path
#=========================================================================
def getPath(path):
    shapes_dir = os.path.join(os.getenv("DATA_DIR"), "Shapes")
    return os.path.join(shapes_dir, path)

###
### SHAPER component
###

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(147.3929660820667, 79.1580464539047, 58.09363834643254, 79.1580464539047)
SketchLine_2 = Sketch_1.addLine(58.09363834643254, 79.1580464539047, 58.09363834643254, 8.969712804393431)
SketchLine_3 = Sketch_1.addLine(58.09363834643254, 8.969712804393431, 147.3929660820667, 8.969712804393431)
SketchLine_4 = Sketch_1.addLine(147.3929660820667, 8.969712804393431, 147.3929660820667, 79.1580464539047)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 150, 0)
Extrusion_1.setName("Box")
Extrusion_1.result().setName("Box")
Group_1 = model.addGroup(Part_1_doc, "Vertices", [model.selection("VERTEX", "[Box/Generated_Face&Sketch_1/SketchLine_1][Box/Generated_Face&Sketch_1/SketchLine_2][Box/To_Face]"), model.selection("VERTEX", "[Box/Generated_Face&Sketch_1/SketchLine_3][Box/Generated_Face&Sketch_1/SketchLine_4][Box/To_Face]")])
Group_1.setName("VerticesGroup")
Group_1.result().setName("VerticesGroup")
Group_2 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Box/Generated_Face&Sketch_1/SketchLine_4"), model.selection("FACE", "Box/Generated_Face&Sketch_1/SketchLine_3")])
Group_2.setName("FacesGroup")
Group_2.result().setName("FacesGroup")
Field_1 = model.addField(Part_1_doc, 1, "DOUBLE", 3, ["Comp 1", "Comp 2", "Comp 3"], [model.selection("EDGE", "[Box/Generated_Face&Sketch_1/SketchLine_2][Box/Generated_Face&Sketch_1/SketchLine_3]")])
Field_1.setName("MyField")
Field_1.result().setName("MyField")
Field_1.addStep(0, 0, [[0, 0, 0], [1, 1.5, 37.2]])

model.end()

###
### SHAPERSTUDY component
###

if 'model' in globals():
  model.publishToShaperStudy()
import SHAPERSTUDY
Box, VerticesGroup, FacesGroup, MyField, = SHAPERSTUDY.shape(model.featureStringId(Extrusion_1))
Box_1, VerticesGroup_1, FacesGroup_1, MyField_1, = SHAPERSTUDY.archive(Box, getPath("Xao/TestShaperStudy2.xao"))
###
### SMESH component
###

import  SMESH, SALOMEDS
from salome.smesh import smeshBuilder

smesh = smeshBuilder.New()
#smesh.SetEnablePublish( False ) # Set to False to avoid publish in study if not needed or in some particular situations:
                                 # multiples meshes built in parallel, complex and numerous mesh edition (performance)

Mesh_1 = smesh.Mesh(Box)
Regular_1D = Mesh_1.Segment()
Number_of_Segments_1 = Regular_1D.NumberOfSegments(4)
MEFISTO_2D = Mesh_1.Triangle(algo=smeshBuilder.MEFISTO)
Length_From_Edges_1 = MEFISTO_2D.LengthFromEdges()
MEFISTO_2D_1 = Mesh_1.Triangle(algo=smeshBuilder.MEFISTO,geom=FacesGroup)
Max_Element_Area_1 = MEFISTO_2D_1.MaxElementArea(2)
Mesh_1.Compute()
Mesh_1.GetMesh().ReplaceShape( Box_1 )
SHAPERSTUDY.breakLinkForSubElements(salome.ObjectToSObject(Mesh_1.GetMesh()), Box_1)
Mesh_1.Clear()
Sub_mesh_1 = MEFISTO_2D_1.GetSubMesh()


## Set names of Mesh objects
smesh.SetName(Regular_1D.GetAlgorithm(), 'Regular_1D')
smesh.SetName(MEFISTO_2D.GetAlgorithm(), 'MEFISTO_2D')
smesh.SetName(Number_of_Segments_1, 'Number of Segments_1')
smesh.SetName(Max_Element_Area_1, 'Max. Element Area_1')
smesh.SetName(Length_From_Edges_1, 'Length From Edges_1')
smesh.SetName(Mesh_1.GetMesh(), 'Mesh_1')
smesh.SetName(Sub_mesh_1, 'Sub-mesh_1')

assert(Mesh_1.Compute())
