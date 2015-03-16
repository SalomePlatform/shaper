// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintVertical.h
// Created: 16 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintVertical_H_
#define SketchPlugin_ConstraintVertical_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_ConstraintBase.h"

/** \class SketchPlugin_ConstraintVertical
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint verticality of a line
 *
 *  This constraint has one attribute SketchPlugin_Constraint::ENTITY_A(),
 *  which specifies a line to be vertical
 */
class SketchPlugin_ConstraintVertical : public SketchPlugin_ConstraintBase
{
 public:
  /// Vertical constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_VERTICAL_ID("SketchConstraintVertical");
    return MY_CONSTRAINT_VERTICAL_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintVertical::ID();
    return MY_KIND;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintVertical();
};

#endif
