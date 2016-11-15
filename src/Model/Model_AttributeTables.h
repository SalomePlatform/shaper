// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeTables.h
// Created:     14 Nov 2016
// Author:      Mikhail Ponikarov

#ifndef Model_AttributeTables_H_
#define Model_AttributeTables_H_

#include "Model.h"

#include <ModelAPI_AttributeTables.h>

#include <TDF_Label.hxx>
#include <TDataStd_IntegerList.hxx>

#include <string>

/// \class Model_AttributeTables
/// \ingroup DataModel
/// \brief API for the attribute that contains tables of some values type.
///
/// The type of values can be changed. But all the values in the tables must have the same one
/// type. The currently allowed types now are: Boolean, Integer, Double, String.
/// By default there is only one table, but it may be increased/decreased by adding/removing 
/// tables one by one.
/// The number of rows and columns are equal in all tables. If table, row or column is added,
/// the previous values are kept unchanged. New cells are filled by zero, false or empty strings.
class Model_AttributeTables: public ModelAPI_AttributeTables
{
public:
  /// Returns the number of rows in the table
  MODEL_EXPORT virtual int rows();
  /// Returns the number of columns in the table
  MODEL_EXPORT virtual int columns();
  /// Returns the number of tables
  MODEL_EXPORT virtual int tables();

  /// Sets the new size of the tables set. This method tries to keep old values if number of
  /// rows, columns or tables is increased.
  MODEL_EXPORT virtual void setSize(
    const int theRows, const int theColumns, const int theTables = 1);

  /// Defines the tyoe of values in the table. If it differs from the current, erases the content.
  MODEL_EXPORT virtual void setType(ValueType theType);
  /// Defines the tyoe of values in the table. If it differs from the current, erases the content.
  MODEL_EXPORT virtual const ValueType& type(ValueType) const;
  /// Defines the value by the index in the tables set (indexes are zero-based).
  MODEL_EXPORT virtual void setValue(
    const Value theValue, const int theRow, const int theColumn, const int theTable = 0);
  /// Returns the value by the index (indexes are zero-based).
  MODEL_EXPORT virtual Value value(
    const int theRow, const int theColumn, const int theTable = 0);

protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeTables(TDF_Label& theLabel);
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

private:
  /// The OCCT array that keeps all values. Indexes are computed as:
  /// TableNum * NbRows * NbColumns + RowNum * NbColumns + ColNum
  //Handle_TDF_Attribute myArray;

  /// Container that stores properties of the tables set: type, nbtables, nbrows, nbcolumns
  /// If sizes are zero, myArray IsNull and vice-versa
  Handle_TDataStd_IntegerList myProp;

  /// cashed main properties
  int myTables, myRows, myCols;
  /// cashed main properties
  ModelAPI_AttributeTables::ValueType myType;

  /// Stores the label as a field
  TDF_Label myLab;

  friend class Model_Data;
};

#endif
