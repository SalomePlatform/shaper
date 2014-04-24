#include "SketchPlugin_Feature.h"

void SketchPlugin_Feature::setPreview(const boost::shared_ptr<GeomAPI_Shape>& theShape)
{
  myPreview = theShape;
}

const boost::shared_ptr<GeomAPI_Shape>& SketchPlugin_Feature::getPreview() const
{
  return myPreview;
}
