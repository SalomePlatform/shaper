// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintEqual.h
// Created: 16 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintEqual_H_
#define SketchPlugin_ConstraintEqual_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_ConstraintBase.h"

/** \class SketchPlugin_ConstraintEqual
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint specifying equality of lengths of two lines
 *         or equality of radii of two circular arcs (full circles)
 *
 *  This constraint has two attributes:
 *  SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B()
 */
class SketchPlugin_ConstraintEqual : public SketchPlugin_ConstraintBase
{
 public:
  /// Equal constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_EQUAL_ID("SketchConstraintEqual");
    return MY_CONSTRAINT_EQUAL_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintEqual::ID();
    return MY_KIND;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintEqual();
};

#endif
