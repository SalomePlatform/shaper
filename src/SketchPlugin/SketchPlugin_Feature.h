// File:        SketchPlugin_Feature.h
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef SketchPlugin_Feature_HeaderFile
#define SketchPlugin_Feature_HeaderFile

#include "SketchPlugin.h"
#include <ModelAPI_Feature.h>

#include <GeomAPI_Shape.h>

/**\class SketchPlugin_Feature
 * \ingroup DataModel
 * \brief Feature for creation of the new part in PartSet.
 */
class SketchPlugin_Feature: public ModelAPI_Feature
{
public:
 SKETCHPLUGIN_EXPORT virtual const std::shared_ptr<GeomAPI_Shape>& preview() = 0;

protected:
  void setPreview(const std::shared_ptr<GeomAPI_Shape>& theShape); ///< the preview shape

private:
  std::shared_ptr<GeomAPI_Shape> myPreview; ///< the preview shape
};

#endif
