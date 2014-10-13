// File:    SketchPlugin_ConstraintRigid.h
// Created: 13 Oct 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintRigid_H_
#define SketchPlugin_ConstraintRigid_H_

#include "SketchPlugin.h"
#include "SketchPlugin_ConstraintBase.h"
#include <SketchPlugin_Sketch.h>
#include <list>

/** \class SketchPlugin_ConstraintRigid
 *  \ingroup DataModel
 *  \brief Feature for creation of a new constraint which defines immovable object
 *
 *  This constraint has one attribute:
 *  SketchPlugin_Constraint::ENTITY_A() (any object, i.e. point, line, circle, arc)
 */
class SketchPlugin_ConstraintRigid : public SketchPlugin_ConstraintBase
{
 public:
  /// Length constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_RIGID_ID("SketchConstraintRigid");
    return MY_CONSTRAINT_RIGID_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintRigid::ID();
    return MY_KIND;
  }

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintRigid();
};

#endif
