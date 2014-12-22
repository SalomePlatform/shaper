#=========================================================================
# Creation of the circular Sketch, then Extrusion (cylinder), 
# then export to the old GEOM and usage of SMESH for meshing.
# Based on SALOME 7.4.0 and NewGEOM version: master 18Dec2014.
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

# Create a part for extrusion
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()

aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

#=========================================================================
# Create a sketch circle to extrude
#=========================================================================
aSession.startOperation()
aSketchFeature = modelAPI_CompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
diry = geomDataAPI_Dir(aSketchFeature.attribute("DirY"))
diry.setValue(0, 1, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
# Create circle
aSketchCircle = aSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aCircleRadius = aSketchCircle.real("CircleRadius")
anCircleCentr.setValue(50., 50)
aCircleRadius.setValue(20.)
aSession.finishOperation()

#=========================================================================
# Make extrusion on circle
#=========================================================================
# Build shape from sketcher results
aSketchResult = aSketchFeature.firstResult()
aSketchEdges = modelAPI_ResultConstruction(aSketchResult).shape()
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin")).pnt()
dirX = geomDataAPI_Dir(aSketchFeature.attribute("DirX")).dir()
dirY = geomDataAPI_Dir(aSketchFeature.attribute("DirY")).dir()
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm")).dir()
aSketchFaces = ShapeList()
GeomAlgoAPI_SketchBuilder.createFaces(
    origin, dirX, dirY, norm, aSketchEdges, aSketchFaces)

# Create extrusion
anExtrusionFt = aPart.addFeature("Extrusion")
anExtrusionFt.selection("extrusion_face").setValue(
    aSketchResult, aSketchFaces[0])
anExtrusionFt.real("extrusion_size").setValue(50)
anExtrusionFt.boolean("extrusion_reverse").setValue(False)
anExtrusionFt.execute()
aSession.finishOperation()

# Check extrusion results
anExtrusionResult = modelAPI_ResultBody(anExtrusionFt.firstResult())

#==================================================
# Transfer shape to Geom module of Salome
#==================================================
aShape = anExtrusionResult.shape()
aDump = aShape.getShapeStream()

# Load shape to SALOME Geom
import salome
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)
aBrep = geompy.RestoreShape(aDump)
geompy.addToStudy(aBrep, "NewGeomShape")

# Meshing of the Shape
from salome.smesh import smeshBuilder
meshpy = smeshBuilder.New(salome.myStudy)
aMesh = meshpy.Mesh(aBrep)
Regular_1D = aMesh.Segment()
Max_Size_1 = Regular_1D.MaxSize(5)
MEFISTO_2D = aMesh.Triangle(algo=smeshBuilder.MEFISTO)
isDone = aMesh.Compute()
assert (isDone)
meshpy.SetName(aMesh.GetMesh(), 'NewGeomMesh')

