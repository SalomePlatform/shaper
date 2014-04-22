// File:        SketchPlugin_Feature.h
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef SketchPlugin_Feature_HeaderFile
#define SketchPlugin_Feature_HeaderFile

#include "SketchPlugin.h"
#include <ModelAPI_Feature.h>

#include "TopoDS_Shape.hxx"

/**\class SketchPlugin_Feature
 * \ingroup DataModel
 * \brief Feature for creation of the new part in PartSet.
 */
class SketchPlugin_Feature: public ModelAPI_Feature
{
public:
 SKETCHPLUGIN_EXPORT virtual const TopoDS_Shape& preview() = 0;

protected:
  void setPreview(const TopoDS_Shape& theShape); ///< the preview shape

private:
  TopoDS_Shape myPreview; ///< the preview shape
};

#endif
