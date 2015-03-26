from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *


class SketchResult:

    def __init__(self, sketch):
        self.geom = sketch.firstResult()
        self.faces = ShapeList()
        self.edges = modelAPI_ResultConstruction(self.geom).shape()
        self.origin = geomDataAPI_Point(sketch.attribute("Origin")).pnt()
        self.dirX = geomDataAPI_Dir(sketch.attribute("DirX")).dir()
        self.dirY = geomDataAPI_Dir(sketch.attribute("DirY")).dir()
        self.normal = geomDataAPI_Dir(sketch.attribute("Norm")).dir()

    def setEdges(self, edges):
        self.edges = edges

    def geometry(self):
        return self.geom

    def face(self):
        GeomAlgoAPI_SketchBuilder.createFaces(
            self.origin, self.dirX, self.dirY, self.normal, self.edges, self.faces)
        return self.faces[0]
