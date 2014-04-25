// File:        SketchPlugin_Sketch.cxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

using namespace std;

/// the active sketch
boost::shared_ptr<SketchPlugin_Sketch> MY_ACITVE_SKETCH;

// face of the square-face displayed for selection of general plane
const double PLANE_SIZE = 200;

SketchPlugin_Sketch::SketchPlugin_Sketch()
{
}

void SketchPlugin_Sketch::initAttributes()
{
  data()->addAttribute(SKETCH_ATTR_ORIGIN, GeomDataAPI_Point::type());
  data()->addAttribute(SKETCH_ATTR_DIRX, GeomDataAPI_Dir::type());
  data()->addAttribute(SKETCH_ATTR_DIRY, GeomDataAPI_Dir::type());
  data()->addAttribute(SKETCH_ATTR_NORM, GeomDataAPI_Dir::type());
}

void SketchPlugin_Sketch::execute() 
{
}

const boost::shared_ptr<GeomAPI_Shape>& SketchPlugin_Sketch::preview()
{
  std::list<boost::shared_ptr<GeomAPI_Shape> > aFaces;

  addPlane(1, 0, 0, aFaces); // YZ plane
  addPlane(0, 1, 0, aFaces); // XZ plane
  addPlane(0, 0, 1, aFaces); // XY plane
  boost::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aFaces);
  setPreview(aCompound);

  return getPreview();
}

const void SketchPlugin_Sketch::addSub(const boost::shared_ptr<ModelAPI_Feature>& theFeature)
{
  boost::dynamic_pointer_cast<SketchPlugin_Feature>(theFeature)->setSketch(this);
}

void SketchPlugin_Sketch::addPlane(double theX, double theY, double theZ,
                                   std::list<boost::shared_ptr<GeomAPI_Shape> >& theShapes) const
{
  boost::shared_ptr<GeomAPI_Pnt> anOrigin(new GeomAPI_Pnt(0, 0, 0));
  boost::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(theX, theY, theZ));
  boost::shared_ptr<GeomAPI_Shape> aFace = 
    GeomAlgoAPI_FaceBuilder::square(anOrigin, aNormal, PLANE_SIZE);
  theShapes.push_back(aFace);
}

boost::shared_ptr<GeomAPI_Pnt> SketchPlugin_Sketch::to3D(const double theX, const double theY)
{
  boost::shared_ptr<GeomDataAPI_Point> aC = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point>(data()->attribute(SKETCH_ATTR_ORIGIN));
  boost::shared_ptr<GeomDataAPI_Dir> aX = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(data()->attribute(SKETCH_ATTR_DIRX));
  boost::shared_ptr<GeomDataAPI_Dir> aY = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(data()->attribute(SKETCH_ATTR_DIRY));

  return boost::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(
    aC->x() + aX->x() * theX + aY->x() * theY, 
    aC->y() + aX->y() * theX + aY->y() * theY,
    aC->z() + aX->z() * theX + aY->z() * theY));
}
