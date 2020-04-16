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

#ifndef Model_AttributeDouble_H_
#define Model_AttributeDouble_H_

#include "Model.h"
#include "ModelAPI_AttributeDouble.h"

#include <TDF_Label.hxx>

class ModelAPI_ExpressionDouble;

/**\class Model_AttributeDouble
 * \ingroup DataModel
 * \brief Attribute that contains real value with double precision.
 */

class Model_AttributeDouble : public ModelAPI_AttributeDouble
{
  std::shared_ptr<ModelAPI_ExpressionDouble> myExpression;

 public:
  /// Defines the double value
  MODEL_EXPORT virtual void setValue(const double theValue);

  /// Returns the double value
  MODEL_EXPORT virtual double value();

  /// Defines the calculated double value
  MODEL_EXPORT virtual void setCalculatedValue(const double theValue);

  /// Defines the text value
  MODEL_EXPORT virtual void setText(const std::string& theText);

  /// Returns the text value
  MODEL_EXPORT virtual std::string text();

  /// Allows to set expression (text) as invalid (by the parameters listener)
  MODEL_EXPORT virtual void setExpressionInvalid(const bool theFlag);

  /// Returns true if text is invalid
  MODEL_EXPORT virtual bool expressionInvalid();

  /// Allows to set expression (text) error (by the parameters listener)
  MODEL_EXPORT virtual void setExpressionError(const std::string& theError);

  /// Returns an expression error
  MODEL_EXPORT virtual std::string expressionError();

  /// Defines the used parameters
  MODEL_EXPORT virtual void setUsedParameters(const std::set<std::string>& theUsedParameters);

  /// Returns the used parameters
  MODEL_EXPORT virtual std::set<std::string> usedParameters() const;

 protected:
  /// Initializes attributes
  Model_AttributeDouble(TDF_Label& theLabel);
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  /// Resets attribute to deafult state.
  virtual void reset();

  friend class Model_Data;
};

#endif
