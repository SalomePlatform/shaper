// File:    SketchPlugin_Constraint.h
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_Constraint_HeaderFile
#define SketchPlugin_Constraint_HeaderFile

#include "SketchPlugin.h"
#include "SketchPlugin_Feature.h"
#include <SketchPlugin_Sketch.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <list>

/*  Description: 
 *    Each constraint uses a set of parameters. In the SolveSpace library 
 *    these parameters are named "valA", "ptA", "ptB", "entityA", "entityB". 
 *    The "ptA" and "ptB" parameters represents a point in the constraint.
 *    The "entityA" and "entityB" represents any other object (and a point too).
 *    And the "valA" represents a real value.
 *
 *    The attributes below are named independent of the SolveSpace.
 *    Some of them may be unused. 
 *
 *    Also the list of possible attributes is provided to simplify assignment.
 */
/// The value parameter for the constraint
const std::string CONSTRAINT_ATTR_VALUE("ConstraintValue");
/// First entity for the constraint
const std::string CONSTRAINT_ATTR_ENTITY_A("ConstraintEntityA");
/// Second entity for the constraint
const std::string CONSTRAINT_ATTR_ENTITY_B("ConstraintEntityB");
/// Third entity for the constraint
const std::string CONSTRAINT_ATTR_ENTITY_C("ConstraintEntityC");
/// Fourth entity for the constraint
const std::string CONSTRAINT_ATTR_ENTITY_D("ConstraintEntityD");
/// List of constraint attributes
const unsigned int CONSTRAINT_ATTR_SIZE = 4;
const std::string CONSTRAINT_ATTRIBUTES[CONSTRAINT_ATTR_SIZE] = 
                      {CONSTRAINT_ATTR_ENTITY_A, CONSTRAINT_ATTR_ENTITY_B, 
                       CONSTRAINT_ATTR_ENTITY_C, CONSTRAINT_ATTR_ENTITY_D};


/** \class SketchPlugin_Constraint
 *  \ingroup DataModel
 *  \brief Feature for creation of a new constraint between other features.
 *         Base class for all constraints.
 */
class SketchPlugin_Constraint: public SketchPlugin_Feature
{
public:
  /// \brief Returns to which group in the document must be added feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getGroup()
  {static std::string MY_GROUP = SKETCH_KIND; return MY_GROUP;}

  /** \brief Adds sub-feature of the higher level feature (sub-element of the sketch)
   *  \param theFeature sub-feature
   */
  SKETCHPLUGIN_EXPORT virtual const void addSub(
    const boost::shared_ptr<ModelAPI_Feature>& theFeature) {}

protected:
  /// \brief Use plugin manager for features creation
  SketchPlugin_Constraint() {}
};

#endif
