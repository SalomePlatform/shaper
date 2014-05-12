// File:    SketchPlugin_Constraint.h
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_Constraint_HeaderFile
#define SketchPlugin_Constraint_HeaderFile

#include "SketchPlugin.h"
#include "SketchPlugin_Feature.h"
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
 *    The attributes below are named corresponding to the SolveSpace, 
 *    some of them may be unused. The list of used attributes is defined 
 *    inside the certain constraint.
 */
/// The value parameter for the constraint
const std::string CONSTRAINT_ATTR_VALUE("ConstraintValueA");
/// First point for the constraint
const std::string CONSTRAINT_ATTR_POINT_A("ConstraintPointA");
/// Second point for the constraint
const std::string CONSTRAINT_ATTR_POINT_B("ConstraintPointB");
/// First entity for the constraint
const std::string CONSTRAINT_ATTR_ENTITY_A("ConstraintEntityA");
/// Second entity for the constraint
const std::string CONSTRAINT_ATTR_ENTITY_B("ConstraintEntityB");


/** \class SketchPlugin_Constraint
 *  \ingroup DataModel
 *  \brief Feature for creation of a new constraint between other features.
 */
class SketchPlugin_Constraint: public SketchPlugin_Feature
{
public:
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind() 
  {static std::string MY_KIND = "SketchConstraint"; return MY_KIND;}

  /// \brief Returns to which group in the document must be added feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getGroup() 
  {static std::string MY_GROUP = "Sketch"; return MY_GROUP;}

  /** \brief Adds sub-feature of the higher level feature (sub-element of the sketch)
   *  \param theFeature sub-feature
   */
  SKETCHPLUGIN_EXPORT virtual const void addSub(
    const boost::shared_ptr<ModelAPI_Feature>& theFeature) {}

  /** \brief Adds an object of the constraint. The object added by the reference.
   *  \param theReference reference to the feature, which will be constrained
   */
  SKETCHPLUGIN_EXPORT virtual void addConstrainedObject(
    const boost::shared_ptr<ModelAPI_AttributeReference>& theReference);

  /** \brief Adds an object of the constraint. The object added by the reference to its attribute.
   *  \param theReference reference to the attribute feature, which will be constrained
   */
  SKETCHPLUGIN_EXPORT virtual void addConstrainedObject(
    const boost::shared_ptr<ModelAPI_AttributeRefAttr>& theReference);

  /// \brief Use plugin manager for features creation
  SketchPlugin_Constraint() {}

protected:
  /** \brief Returns the list of attributes for the certain type of constraint.
   *  \return names of attributes
   */
  virtual const std::list<std::string>& getAttributesList() const = 0;
};

#endif
