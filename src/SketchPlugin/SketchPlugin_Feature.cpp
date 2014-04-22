#include "SketchPlugin_Feature.h"

/**
 * Returns the sketch preview
 */
const TopoDS_Shape& SketchPlugin_Feature::preview()
{
  return myPreview;
}

/**
 * Set the shape to the internal preview field
 * \param theShape a preview shape
 */
void SketchPlugin_Feature::setPreview(const TopoDS_Shape& theShape)
{
  myPreview = theShape;
}
