// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#ifndef Model_Expression_H_
#define Model_Expression_H_

#include "Model.h"
#include "ModelAPI_Expression.h"

#include <TDataStd_Comment.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>

#include <TDF_Label.hxx>

/**\class Model_Expression
 * \ingroup DataModel
 * \brief Implementation of ModelAPI_Expression.
 */
class Model_Expression : public virtual ModelAPI_Expression
{
 public:
  /// Sets the text of this Expression
  MODEL_EXPORT virtual void setText(const std::wstring& theText);

  /// Returns the text of this Expression
  MODEL_EXPORT virtual std::wstring text() const;

  /// Allows to set expression (text) error (by the parameters listener)
  MODEL_EXPORT virtual void setError(const std::string& theError);

  /// Returns an expression error
  MODEL_EXPORT virtual std::string error();

  /// Defines the used parameters (by the parameters listener)
  MODEL_EXPORT virtual void setUsedParameters(const std::set<std::wstring>& theUsedParameters);

  /// Returns the used parameters
  MODEL_EXPORT virtual std::set<std::wstring> usedParameters() const;

 protected:
  /// Initializes attributes
  Model_Expression(TDF_Label& theLabel);

  Handle_TDataStd_Name myText; ///< Text representation of the attribute (may differ for parameters)
  Handle_TDataStd_Comment myError; ///< Error of expression of the text attribute
  Handle_TDataStd_ExtStringList myUsedParameters; ///< Parameters used in expression
  TDF_Label myLab; ///< if attribute is not initialized, store label here
};


/**\class Model_ExpressionDouble
 * \ingroup DataModel
 * \brief Implementation of ModelAPI_ExpressionDouble.
 */
class Model_ExpressionDouble :
    public Model_Expression, // implementation inheritance
    public ModelAPI_ExpressionDouble
{
 public:
  /// Sets the text of this Expression
  MODEL_EXPORT virtual void setText(const std::wstring& theText) {
    Model_Expression::setText(theText);
  };

  /// Returns the text of this Expression
  MODEL_EXPORT virtual std::wstring text() const {
    return Model_Expression::text();
  };

  /// Allows to set expression (text) error (by the parameters listener)
  MODEL_EXPORT virtual void setError(const std::string& theError) {
    Model_Expression::setError(theError);
  };

  /// Returns an expression error
  MODEL_EXPORT virtual std::string error() {
    return Model_Expression::error();
  };

  /// Defines the used parameters (by the parameters listener)
  MODEL_EXPORT virtual void setUsedParameters(const std::set<std::wstring>& theUsedParameters) {
    Model_Expression::setUsedParameters(theUsedParameters);
  };

  /// Returns the used parameters
  MODEL_EXPORT virtual std::set<std::wstring> usedParameters() const {
    return Model_Expression::usedParameters();
  };

  /// Defines the double value
  MODEL_EXPORT virtual void setValue(const double theValue);

  /// Returns the double value
  MODEL_EXPORT virtual double value();

  /// Allows to set expression (text) as invalid (by the parameters listener)
  MODEL_EXPORT virtual void setInvalid(const bool theFlag);

  /// Returns true if text is invalid
  MODEL_EXPORT virtual bool isInvalid();

 protected:
  /// Initializes attributes
  Model_ExpressionDouble(TDF_Label& theLabel);
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  friend class Model_AttributeDouble;
  friend class Model_Data;

 private:
  Handle_TDataStd_Real myReal; ///< double is Real attribute
};


/**\class Model_ExpressionInteger
 * \ingroup DataModel
 * \brief Implementation of ModelAPI_ExpressionInteger.
 */
class Model_ExpressionInteger :
    public Model_Expression, // implementation inheritance
    public ModelAPI_ExpressionInteger
{
 public:
  /// Sets the text of this Expression
  MODEL_EXPORT virtual void setText(const std::wstring& theText) {
    Model_Expression::setText(theText);
  };

  /// Returns the text of this Expression
  MODEL_EXPORT virtual std::wstring text() const {
    return Model_Expression::text();
  };

  /// Allows to set expression (text) error (by the parameters listener)
  MODEL_EXPORT virtual void setError(const std::string& theError) {
    Model_Expression::setError(theError);
  };

  /// Returns an expression error
  MODEL_EXPORT virtual std::string error() {
    return Model_Expression::error();
  };

  /// Defines the used parameters (by the parameters listener)
  MODEL_EXPORT virtual void setUsedParameters(const std::set<std::wstring>& theUsedParameters) {
    Model_Expression::setUsedParameters(theUsedParameters);
  };

  /// Returns the used parameters
  MODEL_EXPORT virtual std::set<std::wstring> usedParameters() const {
    return Model_Expression::usedParameters();
  };

  /// Defines the integer value
  MODEL_EXPORT virtual void setValue(const int theValue);

  /// Returns the integer value
  MODEL_EXPORT virtual int value();

  /// Allows to set expression (text) as invalid (by the parameters listener)
  MODEL_EXPORT virtual void setInvalid(const bool theFlag);

  /// Returns true if text is invalid
  MODEL_EXPORT virtual bool isInvalid();

 protected:
  /// Initializes attributes
  Model_ExpressionInteger(TDF_Label& theLabel);
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  friend class Model_AttributeInteger;

 private:
  Handle_TDataStd_Integer myInteger;
};

#endif // Model_Expression_H_
