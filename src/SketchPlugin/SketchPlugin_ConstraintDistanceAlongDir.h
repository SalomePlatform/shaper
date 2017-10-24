// Copyright (C) 2017  CEA/DEN, EDF R&D
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

// File:    SketchPlugin_ConstraintDistanceAlongDir.h
// Created: 24 October 2017
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintDistanceAlongDir_H_
#define SketchPlugin_ConstraintDistanceAlongDir_H_

#include <SketchPlugin.h>
#include <SketchPlugin_ConstraintDistance.h>

/** \class SketchPlugin_ConstraintDistanceAlongDir
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint which defines a distance along direction.
 *         The base class for horizontal and vertical constraints.
 *
 *  This constraint has three attributes:
 *  SketchPlugin_Constraint::VALUE(), SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B()
 */
class SketchPlugin_ConstraintDistanceAlongDir : public SketchPlugin_ConstraintDistance
{
public:
  /// attribute name of dimension location type
  inline static const std::string& LOCATION_TYPE_ID()
  {
    static const std::string MY_LOCATION_TYPE_ID("LocationType");
    return MY_LOCATION_TYPE_ID;
  }

  /// attribute name of the distance value shown to the user
  inline static const std::string& DISTANCE_VALUE_ID()
  {
    static const std::string& MY_DISTANCE_VALUE("DistanceValue");
    return MY_DISTANCE_VALUE;
  }

  /// attribute name of the sign of distance
  inline static const std::string& NEGATIVE_TYPE_ID()
  {
    static const std::string MY_NEGATIVE_VALUE("NegativeValue");
    return MY_NEGATIVE_VALUE;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintDistanceAlongDir();

protected:
  /// Returns the current distance between the feature attributes
  virtual double calculateCurrentDistance() = 0;

  /// Update flyout point
  virtual void updateFlyoutPoint() = 0;

protected:
  double myValue;
  bool myValueUpdate;
};

#endif
