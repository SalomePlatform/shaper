// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_MultiRotation.h
// Created: 21 Apr 2015
// Author:  Artem ZHIDKOV

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
 *  The list of created objects contains a number of copies of each object given in
 *  the NUMBER_OF_COPIES_ID() attribute plus 1 (the initial objects are stored into this
 *  attribute too). At the start of the list, there are collected N copies
 *  of first object from initial list, then N copies of second object etc.
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
  /// End point of translation
  inline static const std::string& ANGLE_ID()
  {
    static const std::string MY_ANGLE_ID("MultiRotationAngle");
    return MY_ANGLE_ID;
  }
  /// Number of translated objects
  inline static const std::string& NUMBER_OF_COPIES_ID()
  {
    static const std::string MY_NUMBER_OF_COPIES_ID("MultiRotationCopies");
    return MY_NUMBER_OF_COPIES_ID;
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

  /// \brief Use plugin manager for features creation
  SketchPlugin_MultiRotation();

private:
  ObjectPtr copyFeature(ObjectPtr theObject);
  void rotateFeature(ObjectPtr theInitial, ObjectPtr theTarget,
                     double theCenterX, double theCenterY, double theAngle);
};

#endif
