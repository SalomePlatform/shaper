// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Feature.h
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef SketchPlugin_Feature_H_
#define SketchPlugin_Feature_H_

#include "SketchPlugin.h"
#include <ModelAPI_CompositeFeature.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_AISObject.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>

#include <Config_PropManager.h>

class SketchPlugin_Sketch;
class Handle_AIS_InteractiveObject;

/**\class SketchPlugin_Feature
 * \ingroup Plugins
 * \brief Feature for creation of the new feature in PartSet. This is an abstract class to give
 * an interface to create the sketch feature preview.
 */
class SketchPlugin_Feature : public ModelAPI_Feature
{
 public:
  /// Returns true if this feature must be displayed in the history (top level of Part tree)
  SKETCHPLUGIN_EXPORT virtual bool isInHistory()
  {
    return false;
  }

  /// Returns true of the feature is created basing on the external shape of not-this-sketch object
  SKETCHPLUGIN_EXPORT virtual bool isExternal() const
  {
    return false;
  }

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY) = 0;

  /// Construction result is allways recomuted on the fly
  SKETCHPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}

  /// Returns true is sketch element is under the rigid constraint
  SKETCHPLUGIN_EXPORT virtual bool isFixed() {return false;}

  /// Returns the sketch of this feature
  SketchPlugin_Sketch* sketch();
protected:
  /// Sets the higher-level feature for the sub-feature (sketch for line)
  void setSketch(SketchPlugin_Sketch* theSketch)
  {
    mySketch = theSketch;
  }
  /// initializes mySketch
  SketchPlugin_Feature();

  friend class SketchPlugin_Sketch;

 private:
  std::shared_ptr<GeomAPI_Shape> myPreview;  ///< the preview shape
  SketchPlugin_Sketch* mySketch;  /// sketch that contains this feature
};

#endif
