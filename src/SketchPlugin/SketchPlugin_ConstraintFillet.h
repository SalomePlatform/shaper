// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintFillet.h
// Created: 19 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintFillet_H_
#define SketchPlugin_ConstraintFillet_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_ConstraintBase.h"

/** \class SketchPlugin_ConstraintFillet
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint filleting two objects which have coincident point
 *
 *  This constraint has three attributes:
 *  SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B() for the filleting objects;
 *  SketchPlugin_Constraint::VALUE() contains radius of filleting circular arc
 *
 *  Also the constraint has attribute SketchPlugin_Constraint::ENTITY_C()
 *  which contains created list objects forming the fillet
 */
class SketchPlugin_ConstraintFillet : public SketchPlugin_ConstraintBase
{
 public:
  /// Fillet constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_FILLET_ID("SketchConstraintFillet");
    return MY_CONSTRAINT_FILLET_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintFillet::ID();
    return MY_KIND;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Reimplemented from ModelAPI_Feature::isMacro().
  /// \returns true
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const;

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintFillet();
};

#endif
