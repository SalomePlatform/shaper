// Copyright (C) 2014-2023  CEA, EDF
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

#ifndef SketchPlugin_ConstraintDistance_H_
#define SketchPlugin_ConstraintDistance_H_

#include "SketchPlugin.h"
#include "SketchPlugin_ConstraintBase.h"
#include "SketchPlugin_Sketch.h"
#include "ModelAPI_Data.h"

#include <GeomAPI_ICustomPrs.h>

#include <list>

class SketchPlugin_Line;
class GeomDataAPI_Point2D;

/** \class SketchPlugin_ConstraintDistance
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint which defines a distance
 *         between a point and another feature (point, line, plane or face)
 *
 *  This constraint has three attributes:
 *  SketchPlugin_Constraint::VALUE(), SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B()
 */
class SketchPlugin_ConstraintDistance : public SketchPlugin_ConstraintBase
{
 public:
  /// Distance constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_DISTANCE_ID("SketchConstraintDistance");
    return MY_CONSTRAINT_DISTANCE_ID;
  }

  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintDistance::ID();
    return MY_KIND;
  }

  /// \brief Shows whether the point-line distance should keep its sign
  inline static const std::string& SIGNED()
  {
    static const std::string MY_SIGNED("SignedDistance");
    return MY_SIGNED;
  }

  /// \brief The direction from the first object to the second.
  ///        To change distance value from zero to non-zero correctly.
  inline static const std::string& DIRECTION_ID()
  {
    static const std::string MY_DIRECTION_ID("DistanceDirection");
    return MY_DIRECTION_ID;
  }

  /// attribute name of dimension location type
  inline static const std::string& LOCATION_TYPE_ID()
  {
    static const std::string MY_LOCATION_TYPE_ID("LocationType");
    return MY_LOCATION_TYPE_ID;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Retuns the parameters of color definition in the resources config manager
  SKETCHPLUGIN_EXPORT virtual void colorConfigInfo(std::string& theSection, std::string& theName,
                                                   std::string& theDefault);

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintDistance();

protected:
  /// Returns the current distance between the feature attributes
  virtual double calculateCurrentDistance();

  /// Check the attributes related to distanced points/features are initialized
  bool areAttributesInitialized();

protected:
  bool myFlyoutUpdate; ///< to avoid cyclic dependencies on automatic updates of flyout point
};

#endif
