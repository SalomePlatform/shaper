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

import sys
import salome

salome.salome_init()

###
### SHAPER component
###

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "r", "15")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(25, 25, 15)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchCircle_1.center(), SketchAPI_Point(SketchPoint_1).coordinates(), 25)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchCircle_1.center(), SketchAPI_Point(SketchPoint_1).coordinates(), 25)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], "r")
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, 0)
model.do()

Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Sketch_2 = model.addSketch(Part_2_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_2.addLine(-5.176380902051512, 19.31851652579896, 19.31851652579829, -5.176380902058602)
SketchLine_2 = Sketch_2.addLine(19.31851652579829, -5.176380902058602, -14.14213562374638, -14.14213562374059)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_2.addLine(-14.14213562374638, -14.14213562374059, -5.176380902051512, 19.31851652579896)
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
SketchConstraintEqual_1 = Sketch_2.setEqual(SketchLine_1.result(), SketchLine_3.result())
SketchConstraintEqual_2 = Sketch_2.setEqual(SketchLine_1.result(), SketchLine_2.result())
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchLine_2.startPoint(), SketchAPI_Point(SketchPoint_2).coordinates(), 20, True)
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchLine_3.endPoint(), SketchAPI_Point(SketchPoint_2).coordinates(), 20, True)
SketchConstraintDistance_3 = Sketch_2.setDistance(SketchLine_3.startPoint(), SketchAPI_Point(SketchPoint_2).coordinates(), 20, True)
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_4 = SketchProjection_3.createdFeature()
SketchConstraintAngle_1 = Sketch_2.setAngle(SketchLine_3.result(), SketchLine_4.result(), 75)
model.do()
Extrusion_2 = model.addExtrusion(Part_2_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 30, 0)
Fillet_1 = model.addFillet(Part_2_doc, [model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/To_Face]")], 2)
model.do()

Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()
Sketch_3 = model.addSketch(Part_3_doc, model.defaultPlane("XOZ"))
SketchLine_5 = Sketch_3.addLine(52.5, 35, 22.5, 35)
SketchLine_6 = Sketch_3.addLine(22.5, 35, 22.5, 15)
SketchLine_7 = Sketch_3.addLine(22.5, 15, 52.5, 15)
SketchLine_8 = Sketch_3.addLine(52.5, 15, 52.5, 35)
SketchConstraintCoincidence_4 = Sketch_3.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_5 = Sketch_3.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_6 = Sketch_3.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_7 = Sketch_3.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_1 = Sketch_3.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_1 = Sketch_3.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_2 = Sketch_3.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_2 = Sketch_3.setVertical(SketchLine_8.result())
SketchLine_9 = Sketch_3.addLine(0, 0, 52.5, 35)
SketchLine_9.setAuxiliary(True)
SketchProjection_4 = Sketch_3.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_3 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_8 = Sketch_3.setCoincident(SketchLine_9.startPoint(), SketchPoint_3.result())
SketchConstraintCoincidence_9 = Sketch_3.setCoincident(SketchLine_5.startPoint(), SketchLine_9.endPoint())
SketchConstraintCoincidence_10 = Sketch_3.setCoincident(SketchLine_6.endPoint(), SketchLine_9.result())
SketchConstraintLength_1 = Sketch_3.setLength(SketchLine_8.result(), 20)
SketchConstraintLength_2 = Sketch_3.setLength(SketchLine_5.result(), 30)
SketchConstraintDistanceVertical_2 = Sketch_3.setVerticalDistance(SketchLine_7.endPoint(), SketchAPI_Point(SketchPoint_3).coordinates(), 15)
model.do()
Revolution_1 = model.addRevolution(Part_3_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection("EDGE", "PartSet/OX"), 45, 0)
model.end()

###
### SHAPERSTUDY component
###

if 'model' in globals():
  model.publishToShaperStudy()
import SHAPERSTUDY
Extrusion_1_1, = SHAPERSTUDY.shape(model.featureStringId(Extrusion_1))
Fillet_1_1, = SHAPERSTUDY.shape(model.featureStringId(Fillet_1))
Revolution_1_1, = SHAPERSTUDY.shape(model.featureStringId(Revolution_1))
###
### SMESH component
###

import  SMESH, SALOMEDS
from salome.smesh import smeshBuilder

smesh = smeshBuilder.New()
#smesh.SetEnablePublish( False ) # Set to False to avoid publish in study if not needed or in some particular situations:
                                 # multiples meshes built in parallel, complex and numerous mesh edition (performance)

Mesh_1 = smesh.Mesh(Fillet_1_1)
Regular_1D = Mesh_1.Segment()
Number_of_Segments_1 = Regular_1D.NumberOfSegments(7,None,[])
MEFISTO_2D = Mesh_1.Triangle(algo=smeshBuilder.MEFISTO)
Max_Element_Area_1 = MEFISTO_2D.MaxElementArea(20)
isDone = Mesh_1.Compute()
assert(isDone)
Mesh_2 = smesh.Mesh(Extrusion_1_1)
Cartesian_3D = Mesh_2.BodyFitted()
Body_Fitting_Parameters_1 = Cartesian_3D.SetGrid([ [ '4.3589' ], [ 0, 1 ]],[ [ '4.3589' ], [ 0, 1 ]],[ [ '4.3589' ], [ 0, 1 ]],4,0)
Body_Fitting_Parameters_1.SetFixedPoint( SMESH.PointStruct ( 0, 0, 0 ), 1 )
Body_Fitting_Parameters_1.SetAxesDirs( SMESH.DirStruct( SMESH.PointStruct ( 1, 0, 0 )), SMESH.DirStruct( SMESH.PointStruct ( 0, 1, 0 )), SMESH.DirStruct( SMESH.PointStruct ( 0, 0, 1 )) )
isDone = Mesh_2.Compute()
assert(isDone)
Mesh_3 = smesh.Mesh(Revolution_1_1)
status = Mesh_3.AddHypothesis(Number_of_Segments_1)
Regular_1D_1 = Mesh_3.Segment()
isDone = Mesh_3.Compute()
assert(isDone)

## Set names of Mesh objects
smesh.SetName(Regular_1D.GetAlgorithm(), 'Regular_1D')
smesh.SetName(MEFISTO_2D.GetAlgorithm(), 'MEFISTO_2D')
smesh.SetName(Cartesian_3D.GetAlgorithm(), 'Cartesian_3D')
smesh.SetName(Mesh_1.GetMesh(), 'Mesh_1')
smesh.SetName(Mesh_2.GetMesh(), 'Mesh_2')
smesh.SetName(Mesh_3.GetMesh(), 'Mesh_3')
smesh.SetName(Body_Fitting_Parameters_1, 'Body Fitting Parameters_1')
smesh.SetName(Max_Element_Area_1, 'Max. Element Area_1')
smesh.SetName(Number_of_Segments_1, 'Number of Segments_1')

# check the SHAPER study objects generated names
assert(Extrusion_1_1.GetName() == "Extrusion_1_1")
assert(Fillet_1_1.GetName() == "Fillet_1_1")
assert(Revolution_1_1.GetName() == "Revolution_1_1")
# check the smesh mesh is computed correctly
assert(Mesh_1.NbNodes() == 235)
assert(Mesh_2.NbNodes() == 168)
assert(Mesh_3.NbNodes() == 80)
