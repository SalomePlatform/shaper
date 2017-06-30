// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef SketchPlugin_Fillet_H_
#define SketchPlugin_Fillet_H_

#include "SketchPlugin.h"

#include "SketchPlugin_SketchEntity.h"

#include <GeomAPI_IPresentable.h>
#include <GeomDataAPI_Point2D.h>

class GeomAPI_XY;

/// \class SketchPlugin_Fillet
/// \ingroup Plugins
/// \brief Feature for creation of a new arc filleting two objects which have
/// coincident point.
class SketchPlugin_Fillet: public SketchPlugin_SketchEntity, public GeomAPI_IPresentable
{
 public:
  /// Feature kind.
  inline static const std::string& ID()
  {
    static const std::string MY_FEATURE_ID("SketchFillet");
    return MY_FEATURE_ID;
  }

  /// Attribute name of fillet point.
  inline static const std::string& FILLET_POINT_ID()
  {
    static const std::string ID("fillet_point");
    return ID;
  }

  /// \return the kind of a feature.
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Fillet::ID();
    return MY_KIND;
  }

  /// \brief Creates a new part document if needed.
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes.
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Reimplemented from ModelAPI_Feature::isMacro().
  /// \returns true
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const {return true;}

  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const {return false;}

  /// Reimplemented from SketchPlugin_Feature::move().
  /// Do nothing.
  SKETCHPLUGIN_EXPORT virtual void move(const double, const double) {}

  /// \brief Use plugin manager for features creation
  SketchPlugin_Fillet();

private:
  bool calculateFilletParameters();

  bool findFeaturesContainingFilletPoint(std::shared_ptr<GeomDataAPI_Point2D> theFilletPoint);

private:
    FeaturePtr myBaseFeatures[2];
    std::string myFeatAttributes[4]; // attributes of features
    bool myIsReversed;
    bool myIsNotInversed[2]; // indicates which point the features share
    bool myFilletCreated;
    std::shared_ptr<GeomAPI_XY> myCenterXY;
    std::shared_ptr<GeomAPI_XY> myTangentXY1;
    std::shared_ptr<GeomAPI_XY> myTangentXY2;
};

#endif
