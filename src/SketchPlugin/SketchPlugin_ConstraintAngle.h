// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_ConstraintAngle_H_
#define SketchPlugin_ConstraintAngle_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_ConstraintBase.h"

/** \class SketchPlugin_ConstraintAngle
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint fix angle between two lines
 *
 *  This constraint has two attributes:
 *  SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B()
 */
class SketchPlugin_ConstraintAngle : public SketchPlugin_ConstraintBase
{
public:
  /// Angle constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_ANGLE_ID("SketchConstraintAngle");
    return MY_CONSTRAINT_ANGLE_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintAngle::ID();
    return MY_KIND;
  }

  /// attribute name of operation type
  inline static const std::string& TYPE_ID()
  {
    static const std::string MY_TYPE_ID("AngleType");
    return MY_TYPE_ID;
  }
  /// attribute name of previous value of operation type
  inline static const std::string& PREV_TYPE_ID()
  {
    static const std::string MY_TYPE_ID("AngleTypePrevious");
    return MY_TYPE_ID;
  }

  /// attribute name of operation type
  inline static const std::string& ANGLE_VALUE_ID()
  {
    static const std::string MY_ANGLE_VALUE_ID("AngleValue");
    return MY_ANGLE_VALUE_ID;
  }

  /// attribute name indicating the first line is reversed
  inline static const std::string& ANGLE_REVERSED_FIRST_LINE_ID()
  {
    static const std::string MY_ANGLE_REVERSED_ID("AngleReversedLine1");
    return MY_ANGLE_REVERSED_ID;
  }
  /// attribute name indicating the second line is reversed
  inline static const std::string& ANGLE_REVERSED_SECOND_LINE_ID()
  {
    static const std::string MY_ANGLE_REVERSED_ID("AngleReversedLine2");
    return MY_ANGLE_REVERSED_ID;
  }

  /// attribute name of dimension location type
  inline static const std::string& LOCATION_TYPE_ID()
  {
    static const std::string MY_LOCATION_TYPE_ID("LocationType");
    return MY_LOCATION_TYPE_ID;
  }

  /// attribute name indicating the first point selected
  inline static const std::string& SELECTED_FIRST_POINT_ID()
  {
    static const std::string MY_SELECTED_FIRST_POINT_ID("SelectedPointA");
    return MY_SELECTED_FIRST_POINT_ID;
  }

  /// attribute name indicating the second point selected
  inline static const std::string& SELECTED_SECOND_POINT_ID()
  {
    static const std::string MY_SELECTED_SECOND_POINT_ID("SelectedPointB");
    return MY_SELECTED_SECOND_POINT_ID;
  }

public:
  static const int THE_VERSION_0 = 0;
  static const int THE_VERSION_1 = 20191210;

  /// Attribute name of the version of Angle feature
  inline static const std::string& VERSION_ID()
  {
    static const std::string MY_VERSION_ID("version");
    return MY_VERSION_ID;
  }

public:
  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Computes the attribute value on the base of other attributes if the value can be computed
  /// \param theAttributeId an attribute index to be computed
  /// \return a boolean value about it is computed
  SKETCHPLUGIN_EXPORT virtual bool compute(const std::string& theAttributeId);

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Retuns the parameters of color definition in the resources config manager
  SKETCHPLUGIN_EXPORT virtual void colorConfigInfo(std::string& theSection, std::string& theName,
                                                   std::string& theDefault);

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintAngle();

protected:
  /// Calculate current value of the angle
  void calculateAngle();

  /// Compute the position of the angle presentation (the quarter selected by the user)
  void calculateAnglePosition();

  /// Converts the angle value according to the current angle type and sketch plane normal.
  /// The in/out angle is in degree.
  /// \param theAngle a source for the calculated angle
  /// \param a double angle value
  double getAngleForType(double theAngle, bool isReversed1 = false, bool isReversed2 = false);

  /// Update value of ANGLE_VALUE attribute according to the current type
  void updateAngleValue();

  /// Update parameters of the Angle to meet requirements for the latest version
  void updateVersion();

private:
  bool myFlyoutUpdate; ///< to avoid cyclic dependencies on automatic updates of flyout point
};

#endif
