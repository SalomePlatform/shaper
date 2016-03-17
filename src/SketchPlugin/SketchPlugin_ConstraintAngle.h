// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintAngle.h
// Created: 19 August 2015
// Author:  Artem ZHIDKOV

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

  /// attribute name of operation type
  inline static const std::string& ANGLE_VALUE_ID()
  {
    static const std::string MY_ANGLE_VALUE_ID("AngleValue");
    return MY_ANGLE_VALUE_ID;
  }

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

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

  /// Calculate current value of the angle
  double calculateAngle();

  /// Update value of VALUE attribute by the combination of the current angle type and angle value
  void updateAngleValue();

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintAngle();

private:
  bool myFlyoutUpdate; ///< to avoid cyclic dependencies on automatic updates of flyout point
};

#endif
