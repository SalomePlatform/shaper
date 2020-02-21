#!/usr/bin/env python

###
### This file is generated automatically by SALOME v9.4.0 with dump python functionality
###

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
Param_H = model.addParameter(Part_1_doc, "h", "20")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(75.09305815883349, 77.46634028730745, 23.30071943952229, 77.46634028730745)
SketchLine_2 = Sketch_1.addLine(23.30071943952229, 77.46634028730745, 23.30071943952229, 23.93663846925804)
SketchLine_3 = Sketch_1.addLine(23.30071943952229, 23.93663846925804, 75.09305815883349, 23.93663846925804)
SketchLine_4 = Sketch_1.addLine(75.09305815883349, 23.93663846925804, 75.09305815883349, 77.46634028730745)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), "h", 0)

model.end()

###
### SHAPERSTUDY component
###

if 'model' in globals():
  model.publishToShaperStudy()
import SHAPERSTUDY
Extrusion_1_1, = SHAPERSTUDY.shape(model.featureStringId(Extrusion_1))
Extrusion_1_1_1, = SHAPERSTUDY.archive(Extrusion_1_1, getPath("Xao/TestShaperStudy2.xao"))
###
### SMESH component
###

import  SMESH, SALOMEDS
from salome.smesh import smeshBuilder

smesh = smeshBuilder.New()
#smesh.SetEnablePublish( False ) # Set to False to avoid publish in study if not needed or in some particular situations:
                                 # multiples meshes built in parallel, complex and numerous mesh edition (performance)

Mesh_1 = smesh.Mesh(Extrusion_1_1)
Cartesian_3D = Mesh_1.BodyFitted()
Body_Fitting_Parameters_1 = Cartesian_3D.SetGrid([ [ '7.71225' ], [ 0, 1 ]],[ [ '7.71225' ], [ 0, 1 ]],[ [ '7.71225' ], [ 0, 1 ]],4,0)
Body_Fitting_Parameters_1.SetFixedPoint( SMESH.PointStruct ( 0, 0, 0 ), 1 )
Body_Fitting_Parameters_1.SetAxesDirs( SMESH.DirStruct( SMESH.PointStruct ( 1, 0, 0 )), SMESH.DirStruct( SMESH.PointStruct ( 0, 1, 0 )), SMESH.DirStruct( SMESH.PointStruct ( 0, 0, 1 )) )
isDone = Mesh_1.Compute()
Mesh_2 = smesh.Mesh(Extrusion_1_1)
Regular_1D = Mesh_2.Segment()
Number_of_Segments_1 = Regular_1D.NumberOfSegments(7)
MEFISTO_2D = Mesh_2.Triangle(algo=smeshBuilder.MEFISTO)
Max_Element_Area_1 = MEFISTO_2D.MaxElementArea(20)
SHAPERSTUDY.breakLinkForSubElements(salome.ObjectToSObject(Mesh_2.GetMesh()), Extrusion_1_1_1)
Mesh_2.GetMesh().ReplaceShape( Extrusion_1_1_1 )
isDone = Mesh_2.Compute()


## Set names of Mesh objects
smesh.SetName(Cartesian_3D.GetAlgorithm(), 'Cartesian_3D')
smesh.SetName(Regular_1D.GetAlgorithm(), 'Regular_1D')
smesh.SetName(MEFISTO_2D.GetAlgorithm(), 'MEFISTO_2D')
smesh.SetName(Mesh_1.GetMesh(), 'Mesh_1')
smesh.SetName(Mesh_2.GetMesh(), 'Mesh_2')
smesh.SetName(Max_Element_Area_1, 'Max. Element Area_1')
smesh.SetName(Body_Fitting_Parameters_1, 'Body Fitting Parameters_1')
smesh.SetName(Number_of_Segments_1, 'Number of Segments_1')

# check on update of the initial box Mesh_1 computed on it is changed, Mesh_2 on dead shape is unchanged
aNb1 = Mesh_1.NbElements()
aNb2 = Mesh_2.NbElements()
model.begin()
Param_H.setValue(50)
model.end()
model.publishToShaperStudy()
assert(Mesh_1.Compute())
assert(Mesh_1.NbElements() > aNb1)
assert(Mesh_2.Compute())
assert(Mesh_2.NbElements() == aNb2)
