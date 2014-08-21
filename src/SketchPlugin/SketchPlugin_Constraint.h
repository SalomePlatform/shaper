// File:    SketchPlugin_Constraint.h
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_Constraint_H_
#define SketchPlugin_Constraint_H_

#include <SketchPlugin_Feature.h>

#include <string>

/// Size of the list of constraint attributes
const unsigned int CONSTRAINT_ATTR_SIZE = 4;

/** \class SketchPlugin_Constraint
 *  \brief Abstract interface to the SketchPlugin_ConstraintBase
 *         For more info see: SketchPlugin_ConstraintBase.h
 */
class SketchPlugin_Constraint : public SketchPlugin_Feature
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
  inline static const std::string& ATTRIBUTE(const int theNumber)
  {
    switch (theNumber) {
      case 0:
        return ENTITY_A();
      case 1:
        return ENTITY_B();
      case 2:
        return ENTITY_C();
      case 3:
        return ENTITY_D();
      default:
        break;
    }
    static const std::string EMPTY_STRING("");
    return EMPTY_STRING;
  }

 protected:
  /// \brief Use plugin manager for features creation
  SketchPlugin_Constraint()
  {
  }
};

#endif
