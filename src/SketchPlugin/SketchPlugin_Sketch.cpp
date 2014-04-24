// File:        SketchPlugin_Sketch.cxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

using namespace std;

// face of the square-face displayed for selection of general plane
const double PLANE_SIZE = 200;

SketchPlugin_Sketch::SketchPlugin_Sketch()
{
}

void SketchPlugin_Sketch::initAttributes()
{
  //data()->addAttribute(PART_ATTR_DOC_REF, ModelAPI_AttributeDocRef::type());
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

void SketchPlugin_Sketch::addPlane(double theX, double theY, double theZ,
                                   std::list<boost::shared_ptr<GeomAPI_Shape> >& theShapes) const
{
  boost::shared_ptr<GeomAPI_Pnt> anOrigin(new GeomAPI_Pnt(0, 0, 0));
  boost::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(theX, theY, theZ));
  boost::shared_ptr<GeomAPI_Shape> aFace = 
    GeomAlgoAPI_FaceBuilder::square(anOrigin, aNormal, PLANE_SIZE);
  theShapes.push_back(aFace);
}
