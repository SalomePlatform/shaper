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

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Retuns the parameters of color definition in the resources config manager
  SKETCHPLUGIN_EXPORT virtual void colorConfigInfo(std::string& theSection, std::string& theName,
                                                   std::string& theDefault);

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintDistance();

protected:
  /// Returns the current distance between the feature attributes
  double calculateCurrentDistance();

private:
  bool myFlyoutUpdate; ///< to avoid cyclic dependencies on automatic updates of flyout point
};

#endif
