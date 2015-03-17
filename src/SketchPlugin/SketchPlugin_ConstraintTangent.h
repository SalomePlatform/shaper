// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintTangent.h
// Created: 16 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintTangent_H_
#define SketchPlugin_ConstraintTangent_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_ConstraintBase.h"

/** \class SketchPlugin_ConstraintTangent
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint tangency between line and arc 
 *         or between two circular arcs. The used features must have shared vertex
 *
 *  This constraint has two attributes:
 *  SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B()
 */
class SketchPlugin_ConstraintTangent : public SketchPlugin_ConstraintBase
{
 public:
  /// Equal constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_TANGENT_ID("SketchConstraintTangent");
    return MY_CONSTRAINT_TANGENT_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintTangent::ID();
    return MY_KIND;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintTangent();
};

#endif
