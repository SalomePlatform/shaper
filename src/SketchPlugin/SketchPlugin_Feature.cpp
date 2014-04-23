#include "SketchPlugin_Feature.h"

/**
 * Returns the sketch preview
 */
const boost::shared_ptr<GeomAPI_Shape>& SketchPlugin_Feature::preview()
{
  return myPreview;
}

/**
 * Set the shape to the internal preview field
 * \param theShape a preview shape
 */
void SketchPlugin_Feature::setPreview(const boost::shared_ptr<GeomAPI_Shape>& theShape)
{
  myPreview = theShape;
}
