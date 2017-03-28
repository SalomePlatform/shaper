# Author: Sergey Pokhodenko
# Copyright (C) 2014-20xx CEA/DEN, EDF R&D

import ModelHighAPI
import GeomDataAPI

def addPolyline(sketch, *coords):
    """Add a poly-line to sketch.

    The end of consecutive segments are defined as coincident.
    """
    c0 = coords[0]
    c1 = coords[1]
    polyline = []
    line_1 = sketch.addLine(c0, c1)
    polyline.append(line_1)
    # Adding and connecting next lines
    for c2 in coords[2:]:
        line_2 = sketch.addLine(c1, c2)
        sketch.setCoincident(line_1.endPoint(), line_2.startPoint())
        polyline.append(line_2)
        c1 = c2
        line_1 = line_2
    return polyline


def addPolygon(sketch, *coords):
    """Add a polygon to sketch.

    The end of consecutive segments are defined as coincident.
    """
    pg = addPolyline(sketch, *coords)
    # Closing the poly-line supposed being defined by at least 3 points
    c0 = coords[0]
    cn = coords[len(coords) - 1]
    ln = sketch.addLine(cn, c0)
    sketch.setCoincident(
        pg[len(coords) - 2].endPoint(), ln.startPoint()
        )
    sketch.setCoincident(
        ln.endPoint(), pg[0].startPoint()
        )
    pg.append(ln)
    return pg

def dof(sketch):
    """ Extract degrees of freedom for the given sketch
    """
    aSketch = sketch
    if issubclass(type(aSketch), ModelHighAPI.ModelHighAPI_Interface):
        aSketch = sketch.feature()
    return int(filter(str.isdigit, aSketch.string("SolverDOF").value()))

def distancePointPoint(thePoint1, thePoint2):
    aGeomPnt1 = thePoint1
    aGeomPnt2 = thePoint2
    if issubclass(type(thePoint1), GeomDataAPI.GeomDataAPI_Point2D):
        aGeomPnt1 = thePoint1.pnt()
    if issubclass(type(thePoint2), GeomDataAPI.GeomDataAPI_Point2D):
        aGeomPnt2 = thePoint2.pnt()
    return aGeomPnt1.distance(aGeomPnt2)

def lastSubFeature(theSketch, theKind):
    """
    obtains last feature of given kind from the sketch
    """
    for anIndex in range(theSketch.numberOfSubs() - 1, -1, -1):
        aSub = theSketch.subFeature(anIndex)
        if (aSub.getKind() == theKind):
            return aSub
