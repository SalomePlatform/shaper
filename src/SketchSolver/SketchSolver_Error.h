// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Error.h
// Created: 29 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_Error_H_
#define SketchSolver_Error_H_

#include <string>

/** \class SketchSolver_Error
 *  \ingroup Plugins
 *  \brief Collects all sketch solver error' codes
 *         as inline static functions
 */
class SketchSolver_Error
{
 public:
  /// The value parameter for the constraint
  inline static const std::string& CONSTRAINTS()
  {
    static const std::string MY_ERROR_VALUE("Conflicting constraints");
    return MY_ERROR_VALUE;
  }
  /// The entities need to have shared point, but they have not
  inline static const std::string& NO_COINCIDENT_POINTS()
  {
    static const std::string MY_ERROR_VALUE("Objects should have coincident point");
    return MY_ERROR_VALUE;
  }
  /// Attribute of a feature is not initialized
  inline static const std::string& NOT_INITIALIZED()
  {
    static const std::string MY_ERROR_VALUE("Attribute is not initialized");
    return MY_ERROR_VALUE;
  }
  /// Constraint has wrong attributes
  inline static const std::string& INCORRECT_ATTRIBUTE()
  {
    static const std::string MY_ERROR_VALUE("Incorrect attribute");
    return MY_ERROR_VALUE;
  }
  /// Tangency constraint has wrong attributes
  inline static const std::string& INCORRECT_TANGENCY_ATTRIBUTE()
  {
    static const std::string MY_ERROR_VALUE("An arc should be an attribute of tangency constraint");
    return MY_ERROR_VALUE;
  }
  /// Mirror constraint has wrong attributes
  inline static const std::string& INCORRECT_MIRROR_ATTRIBUTE()
  {
    static const std::string MY_ERROR_VALUE("Mirror constraint has wrong attributes");
    return MY_ERROR_VALUE;
  }
};

#endif
