// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef SketchPlugin_Feature_H_
#define SketchPlugin_Feature_H_

#include "SketchPlugin.h"
#include <ModelAPI_CompositeFeature.h>
#include <GeomAPI_Shape.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>

#include <Config_PropManager.h>

class SketchPlugin_Sketch;

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

  /// Returns true if the feature is a copy of other feature
  SKETCHPLUGIN_EXPORT virtual bool isCopy() const
  {
    return false;
  }

  /// Returns true if the feature and the feature results can be displayed
  /// \return true
  SKETCHPLUGIN_EXPORT virtual bool canBeDisplayed() const
  {
    return true;
  }

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

  /// Store current feature of the document if it is not the sub-feature of the current sketch
  void keepCurrentFeature();
  /// Restore current feature of the document after adding new feature to the sketch
  void restoreCurrentFeature();

  friend class SketchPlugin_Sketch;

private:
  std::shared_ptr<GeomAPI_Shape> myPreview;  ///< the preview shape
  SketchPlugin_Sketch* mySketch;  /// sketch that contains this feature

  FeaturePtr myCurrentFeature; /// temporary stored current feature
};

#endif
