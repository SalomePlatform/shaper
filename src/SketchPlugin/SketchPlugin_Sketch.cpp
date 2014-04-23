// File:        SketchPlugin_Sketch.cxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
#include <GeomAlgoAPI_FaceBuilder.h>

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

const shared_ptr<GeomAPI_Shape>& SketchPlugin_Sketch::preview()
{
  if (!SketchPlugin_Feature::preview())
  {

    shared_ptr<GeomAPI_Pnt> anOrigin(new GeomAPI_Pnt(0, 0, 0));
    shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(1, 0, 0));
    shared_ptr<GeomAPI_Shape> aFace = 
      GeomAlgoAPI_FaceBuilder::square(anOrigin, aNormal, PLANE_SIZE);
    setPreview(aFace);
  }
  return SketchPlugin_Feature::preview();
}
