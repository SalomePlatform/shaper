// Copyright (C) 2017-2023  CEA, EDF
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

// File:    SketchPlugin_ConstraintDistanceVertical.h
// Created: 10 May 2017
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintDistanceVertical_H_
#define SketchPlugin_ConstraintDistanceVertical_H_

#include <SketchPlugin.h>
#include <SketchPlugin_ConstraintDistanceAlongDir.h>

/** \class SketchPlugin_ConstraintDistanceVertical
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint which defines a vertical distance between two points.
 *
 *  This constraint has three attributes:
 *  SketchPlugin_Constraint::VALUE(), SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B()
 */
class SketchPlugin_ConstraintDistanceVertical : public SketchPlugin_ConstraintDistanceAlongDir
{
public:
  /// Distance constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_DISTANCE_ID("SketchConstraintDistanceVertical");
    return MY_CONSTRAINT_DISTANCE_ID;
  }

  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintDistanceVertical::ID();
    return MY_KIND;
  }

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintDistanceVertical();

protected:
  /// Returns the current distance between the feature attributes
  virtual double calculateCurrentDistance();

  /// Update flyout point
  virtual void updateFlyoutPoint();
};

#endif
