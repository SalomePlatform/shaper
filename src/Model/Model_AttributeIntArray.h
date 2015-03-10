// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeIntArray.h
// Created:     6 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef MODEL_ATTRIBUTEINTARRAY_H_
#define MODEL_ATTRIBUTEINTARRAY_H_

#include <Model.h>
#include <ModelAPI_AttributeIntArray.h>

#include <TDF_Label.hxx>
#include <TDataStd_IntegerArray.hxx>

#include <string>

/**\class Model_AttributeIntArray
 * \ingroup DataModel
 * \brief API for the attribute that contains several integers in the array inside.
 * Used for RGB color storage for an example. By default size is one, zero-based.
 * Represented as array in OCCT. Empty array means that this attribute does not exists.
 */

class Model_AttributeIntArray : public ModelAPI_AttributeIntArray
{
  /// The OCCT array that keeps all values. 
  Handle_TDataStd_IntegerArray myArray;
  /// Stores the label as a field to set array if size is not null (null array is buggy in OCAF)
  TDF_Label myLab;
 public:

  /// Returns the size of the array (zero means that it is empty)
  MODEL_EXPORT virtual int size();

  /// Sets the new size of the array. The previous data is erased.
  MODEL_EXPORT virtual void setSize(const int theSize);

  /// Defines the value of the array by index [0; size-1]
  MODEL_EXPORT virtual void setValue(const int theIndex,
                                     const int theValue);

  /// Returns the value by the index
  MODEL_EXPORT virtual int value(const int theIndex);


 protected:
  /// Initializes attibutes
  Model_AttributeIntArray(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
