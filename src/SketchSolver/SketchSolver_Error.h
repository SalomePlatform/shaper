// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
    static const std::string MY_ERROR_VALUE("<b>The constraint is conflicting with others. "
      "To fix this, you can either <font color='red'>undo (Ctrl+Z)</font> your operation or "
      "<font color='red'>remove</font> a conflicting constraint.</b>");
    return MY_ERROR_VALUE;
  }
  /// Cyclic dependency of copied features with their originals
  inline static const std::string& INFINITE_LOOP()
  {
    static const std::string MY_ERROR_VALUE(
      "<b>There is a circular reference between copied sketch entities and their originals. "
      "To fix this, you can either <font color='red'>undo (Ctrl+Z)</font> your operation or "
      "<font color='red'>remove</font> wrong constraint.</b>");
    return MY_ERROR_VALUE;
  }
  /// Constraints should use objects instead of features as attributes
  inline static const std::string& NEED_OBJECT_NOT_FEATURE()
  {
    static const std::string
      MY_ERROR_VALUE("Constraint should be based on object instead of feature");
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
  /// Entity is already fixed
  inline static const std::string& ALREADY_FIXED()
  {
    static const std::string MY_ERROR_VALUE("Entity already fixed");
    return MY_ERROR_VALUE;
  }
  /// Tangency constraint has been built with wrong attributes
  /// (for example, line and arc have two coincident points)
  inline static const std::string& TANGENCY_FAILED()
  {
    static const std::string
      MY_ERROR_VALUE("Unable to create tangency constraint on given attributes");
    return MY_ERROR_VALUE;
  }
  /// Crash in SolveSpace
  inline static const std::string& SOLVESPACE_CRASH()
  {
    static const std::string MY_ERROR_VALUE("Caution: SolveSpace crashed! Constraints are wrong");
    return MY_ERROR_VALUE;
  }
  /// Constraint has wrong type
  inline static const std::string& WRONG_CONSTRAINT_TYPE()
  {
    static const std::string MY_ERROR_VALUE("Unsupported type of constraint");
    return MY_ERROR_VALUE;
  }
  /// Sketch contains degenerated geometry
  inline static const std::string& DEGENERATED_GEOMETRY()
  {
    static const std::string MY_ERROR_VALUE(
      "The set of constraints lead to degenerated geometry. "
      "To fix this, you can either undo your operation or "
      "remove a constraint or the degenerated geometry.");
    return MY_ERROR_VALUE;
  }
};

#endif
