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

#ifndef SketchPlugin_MultiRotation_H_
#define SketchPlugin_MultiRotation_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_ConstraintBase.h"

/** \class SketchPlugin_MultiRotation
 *  \ingroup Plugins
 *  \brief Feature for creation of a number of rotated copies of a list of objects
 *
 *  This constraint has two attributes:
 *  SketchPlugin_Constraint::ENTITY_A() for initial list of objects and
 *  SketchPlugin_Constraint::ENTITY_B() for the list of created objects
 *
 *  The list of created objects contains initial and copied objects of each object given. The
 *  number copies is the NUMBER_OF_OBJECTS_ID() minus 1. At the start of the list, there are
 * collected N copies of first object from initial list, then N copies of second object etc.
 */
class SketchPlugin_MultiRotation : public SketchPlugin_ConstraintBase
{
 public:
  /// Multi-rotation kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_ROTATION_ID("SketchMultiRotation");
    return MY_CONSTRAINT_ROTATION_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_MultiRotation::ID();
    return MY_KIND;
  }

  /// List of objects to be mirrored
  inline static const std::string& ROTATION_LIST_ID()
  {
    static const std::string MY_ROTATION_LIST_ID("MultiRotationList");
    return MY_ROTATION_LIST_ID;
  }

  /// Center of rotation
  inline static const std::string& CENTER_ID()
  {
    static const std::string MY_CENTER_ID("MultiRotationCenter");
    return MY_CENTER_ID;
  }
  /// attribute name for first point
  inline static const std::string& ANGLE_TYPE()
  {
    static const std::string ANGLE_TYPE_ATTR("AngleType");
    return ANGLE_TYPE_ATTR;
  }

  /// End point of translation
  inline static const std::string& ANGLE_ID()
  {
    static const std::string MY_ANGLE_ID("MultiRotationAngle");
    return MY_ANGLE_ID;
  }

  /// Total number of objects, initial and translated objects
  inline static const std::string& NUMBER_OF_OBJECTS_ID()
  {
    static const std::string MY_NUMBER_OF_OBJECTS_ID("MultiRotationObjects");
    return MY_NUMBER_OF_OBJECTS_ID;
  }

  /// Name of the flag to reverse rotation
  inline static const std::string& REVERSED_ID()
  {
    static const std::string MY_REVERSED_ID("MultiRotationReversed");
    return MY_REVERSED_ID;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// removes all fields from this feature: results, data, etc
  SKETCHPLUGIN_EXPORT virtual void erase();

  /// \brief Use plugin manager for features creation
  SketchPlugin_MultiRotation();

private:
  ObjectPtr copyFeature(ObjectPtr theObject);
  //void rotateFeature(ObjectPtr theInitial, ObjectPtr theTarget,
  //                   double theCenterX, double theCenterY, double theAngle);

  bool updateFullAngleValue();

  bool isUpdatingAngle;
};

#endif
