// File:    SketchPlugin_Constraint.h
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_Constraint_H_
#define SketchPlugin_Constraint_H_

#include "SketchPlugin.h"
#include "SketchPlugin_Feature.h"
#include <SketchPlugin_Sketch.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefAttr.h>

#include <GeomAPI_IPresentable.h>

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

/// Size of the list of constraint attributes
const unsigned int CONSTRAINT_ATTR_SIZE = 4;

/** \class SketchPlugin_Constraint
 *  \ingroup DataModel
 *  \brief Feature for creation of a new constraint between other features.
 *         Base class for all constraints.
 */
class SketchPlugin_Constraint: public SketchPlugin_Feature, 
                               public GeomAPI_IPresentable
{
public:
  /// The value parameter for the constraint
  inline static const std::string& VALUE()
  {
    static const std::string MY_CONSTRAINT_VALUE("ConstraintValue");
    return MY_CONSTRAINT_VALUE;
  }
  /// The 2D value parameter for the constraint
  inline static const std::string& FLYOUT_VALUE_PNT()
  {
    static const std::string MY_FLYOUT_VALUE_PNT("ConstraintFlyoutValuePnt");
    return MY_FLYOUT_VALUE_PNT;
  }
  /// First entity for the constraint
  inline static const std::string& ENTITY_A()
  {
    static const std::string MY_ENTITY_A("ConstraintEntityA");
    return MY_ENTITY_A;
  }
  /// Second entity for the constraint
  inline static const std::string& ENTITY_B()
  {
    static const std::string MY_ENTITY_B("ConstraintEntityB");
    return MY_ENTITY_B;
  }
  /// Third entity for the constraint
  inline static const std::string& ENTITY_C()
  {
    static const std::string MY_ENTITY_C("ConstraintEntityC");
    return MY_ENTITY_C;
  }
  /// Fourth entity for the constraint
  inline static const std::string& ENTITY_D()
  {
    static const std::string MY_ENTITY_D("ConstraintEntityD");
    return MY_ENTITY_D;
  }

  /// List of constraint attributes
  inline static const std::string& ATTRIBUTE(const int theNumber) {
    switch (theNumber) {
      case 0: return ENTITY_A();
      case 1: return ENTITY_B();
      case 2: return ENTITY_C();
      case 3: return ENTITY_D();
      default: break;
    }
    static const std::string EMPTY_STRING("");
    return EMPTY_STRING;
  }

  /// \brief Returns to which group in the document must be added feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getGroup()
  {static std::string MY_GROUP = SketchPlugin_Sketch::ID(); return MY_GROUP;}

  /** \brief Adds sub-feature of the higher level feature (sub-element of the sketch)
   *  \param theFeature sub-feature
   */
  SKETCHPLUGIN_EXPORT virtual const void addSub(
    const FeaturePtr& theFeature) {}

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual boost::shared_ptr<GeomAPI_AISObject> getAISObject(
                                boost::shared_ptr<GeomAPI_AISObject> thePrevious);

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY) {};

  /// Return the distance between the feature and the point
  /// \param thePoint the point
  virtual double distanceToPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint) { return 0; };

protected:
  /// \brief Use plugin manager for features creation
  SketchPlugin_Constraint() {}
};

#endif
