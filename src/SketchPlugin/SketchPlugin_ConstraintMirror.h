// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintMirror.h
// Created: 17 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintMirror_H_
#define SketchPlugin_ConstraintMirror_H_

#include "SketchPlugin.h"
#include <SketchPlugin_Sketch.h>
#include "SketchPlugin_ConstraintBase.h"

/** \class SketchPlugin_ConstraintMirror
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint mirroring a list of objects regarding to a given line
 *
 *  This constraint has two attributes:
 *  SketchPlugin_Constraint::ENTITY_A() for mirror line and
 *  SketchPlugin_Constraint::ENTITY_B() for the list of objects
 *
 *  Also the constraint has attribute SketchPlugin_Constraint::ENTITY_C()
 *  which contains list of mirrored objects
 */
class SketchPlugin_ConstraintMirror : public SketchPlugin_ConstraintBase
{
 public:
  /// Mirror constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_MIRROR_ID("SketchConstraintMirror");
    return MY_CONSTRAINT_MIRROR_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintMirror::ID();
    return MY_KIND;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintMirror();
};

#endif
