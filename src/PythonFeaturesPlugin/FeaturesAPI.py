from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *

# NOTE : I think this style should be chosen 
# for function as recommended by Python programming
# standards
def build_face_from_sketch (sketch, edges=None) :
  # If no edges have been selected, get the whole sketch 
  # edges
  if edges == None:
    result = sketch.firstResult()
    edges  = modelAPI_ResultConstruction(result).shape()
  
  # Build the face
  origin = geomDataAPI_Point( sketch.attribute("Origin") ).pnt()
  dirX   = geomDataAPI_Dir( sketch.attribute("DirX") ).dir()
  dirY   = geomDataAPI_Dir( sketch.attribute("DirY") ).dir()
  normal = geomDataAPI_Dir( sketch.attribute("Norm") ).dir()
  faces  = ShapeList()
  GeomAlgoAPI_SketchBuilder.createFaces(origin, dirX, dirY, normal, edges, faces)
  return faces[0]

# NOTE : with an optional argument it is not
# a so good idea to put part as last argument
# it would result in a mandatory argument
# put after an optionnal one
def addExtrusion(part, sketch, size, reverse=False, subshapes=None):
  feature = part.addFeature("Extrusion")
  
  # Build apropriate face
  face = build_face_from_sketch(sketch, subshapes)
  # Get sketch result
  sketchResult = sketch.firstResult()
  
  # Set attributes and execute the feature
  feature.selection("extrusion_face").setValue(sketchResult, face)
  feature.real("extrusion_size").setValue(size)
  feature.boolean("extrusion_reverse").setValue(False)
  feature.execute()
  
  return feature
