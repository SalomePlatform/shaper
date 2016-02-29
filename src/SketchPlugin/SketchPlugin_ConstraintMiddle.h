// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintMiddle.h
// Created: 27 Jan 2016
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintMiddle_H_
#define SketchPlugin_ConstraintMiddle_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_ConstraintBase.h"

/** \class SketchPlugin_ConstraintMiddle
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint which places a point in the middle of a line
 *
 *  This constraint has two attributes:
 *  SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B()
 */
class SketchPlugin_ConstraintMiddle : public SketchPlugin_ConstraintBase
{
 public:
  /// Parallel constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_MIDDLE_ID("SketchConstraintMiddle");
    return MY_CONSTRAINT_MIDDLE_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintMiddle::ID();
    return MY_KIND;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintMiddle();
};

#endif
