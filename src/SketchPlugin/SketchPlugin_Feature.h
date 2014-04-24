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
 * \brief Feature for creation of the new feature in PartSet. This is an abstract class to give
 * an interface to create the sketch feature preview.
 */
class SketchPlugin_Feature: public ModelAPI_Feature
{
public:
  /// Returns the sketch preview
  /// \return the built preview
  SKETCHPLUGIN_EXPORT virtual const boost::shared_ptr<GeomAPI_Shape>& preview() = 0;

protected:
  /// Set the shape to the internal preview field
  /// \param theShape a preview shape
  void setPreview(const boost::shared_ptr<GeomAPI_Shape>& theShape); ///< the preview shape
  /// Return the shape from the internal preview field
  /// \return theShape a preview shape
  const boost::shared_ptr<GeomAPI_Shape>& getPreview() const;

private:
  boost::shared_ptr<GeomAPI_Shape> myPreview; ///< the preview shape
};

#endif
