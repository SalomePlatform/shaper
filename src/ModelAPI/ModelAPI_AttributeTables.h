// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef ModelAPI_AttributeTables_H_
#define ModelAPI_AttributeTables_H_

#include <ModelAPI.h>
#include <ModelAPI_Attribute.h>

#include <string>

/// \class ModelAPI_AttributeTables
/// \ingroup DataModel
/// \brief API for the attribute that contains tables of some values type.
///
/// The type of values can be changed. But all the values in the tables must have the same one
/// type. The currently allowed types now are: Boolean, Integer, Double, String.
/// By default there is only one table, but it may be increased/decreased by adding/removing
/// tables one by one.
/// The number of rows and columns are equal in all tables. If table, row or column is added,
/// the previous values are kept unchanged. New cells are filled by zero, false or empty strings.
class ModelAPI_AttributeTables: public ModelAPI_Attribute
{
public:
  /// Type of the value in the table
  enum ValueType {
    BOOLEAN,
    INTEGER,
    DOUBLE,
    STRING
  };

  struct Value {
    bool myBool;
    int myInt;
    double myDouble;
    std::string myStr;
  };

  /// Returns the number of rows in the table
  MODELAPI_EXPORT virtual int rows() = 0;
  /// Returns the number of columns in the table
  MODELAPI_EXPORT virtual int columns() = 0;
  /// Returns the number of tables
  MODELAPI_EXPORT virtual int tables() = 0;

  /// Sets the new size of the tables set. This method tries to keep old values if number of
  /// rows, columns or tables is increased.
  MODELAPI_EXPORT virtual void setSize(
    const int theRows, const int theColumns, const int theTables = 1) = 0;

  /// Defines the tyoe of values in the table. If it differs from the current, erases the content.
  MODELAPI_EXPORT virtual void setType(ValueType theType) = 0;
  /// Defines the tyoe of values in the table. If it differs from the current, erases the content.
  MODELAPI_EXPORT virtual const ValueType& type() const = 0;
  /// Defines the value by the index in the tables set (indexes are zero-based).
  MODELAPI_EXPORT virtual void setValue(
    const Value theValue, const int theRow, const int theColumn, const int theTable = 0) = 0;
  /// Returns the value by the index (indexes are zero-based).
  MODELAPI_EXPORT virtual Value value(
    const int theRow, const int theColumn, const int theTable = 0) = 0;

  /// Returns the value in the format of string (usefull for the python connection)
  MODELAPI_EXPORT virtual std::string valueStr(
    const int theRow, const int theColumn, const int theTable = 0) = 0;

  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string typeId()
  {
    return "Tables";
  }
  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType();
  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeTables();

protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeTables();
};

/// Pointer on double attribute
typedef std::shared_ptr<ModelAPI_AttributeTables> AttributeTablesPtr;

#endif
