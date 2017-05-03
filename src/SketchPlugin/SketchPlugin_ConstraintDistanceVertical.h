// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintDistanceVertical.h
// Created: 10 May 2017
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintDistanceVertical_H_
#define SketchPlugin_ConstraintDistanceVertical_H_

#include <SketchPlugin.h>
#include <SketchPlugin_ConstraintDistance.h>

/** \class SketchPlugin_ConstraintDistanceVertical
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint which defines a vertical distance between two points.
 *
 *  This constraint has three attributes:
 *  SketchPlugin_Constraint::VALUE(), SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B()
 */
class SketchPlugin_ConstraintDistanceVertical : public SketchPlugin_ConstraintDistance
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

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

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
  SketchPlugin_ConstraintDistanceVertical();

protected:
  /// Returns the current distance between the feature attributes
  virtual double calculateCurrentDistance();

private:
  bool myFlyoutUpdate; ///< to avoid cyclic dependencies on automatic updates of flyout point
};

#endif
