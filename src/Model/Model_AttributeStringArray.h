// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeStringArray.h
// Created:     14 Nov 2016
// Author:      Mikhail Ponikarov

#ifndef Model_AttributeStringArray_H_
#define Model_AttributeStringArray_H_

#include <Model.h>
#include <ModelAPI_AttributeStringArray.h>
#include <TDataStd_ExtStringArray.hxx>
#include <TDF_Label.hxx>

/// \class Model_AttributeStringArray
/// \ingroup DataModel
/// \brief API for the attribute that contains several strings in the array inside.
class Model_AttributeStringArray: public ModelAPI_AttributeStringArray
{
public:
  /// Returns the size of the array (zero means that it is empty)
  MODEL_EXPORT virtual int size();

  /// Sets the new size of the array. The previous data is erased.
  MODEL_EXPORT virtual void setSize(const int theSize);

  /// Defines the value of the array by index [0; size-1]
  MODEL_EXPORT virtual void setValue(const int theIndex,
                                     const std::string theValue);

  /// Returns the value by the index
  MODEL_EXPORT virtual std::string value(const int theIndex);

protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeStringArray(TDF_Label& theLabel);

private:
  /// The OCCT array that keeps all values.
  Handle_TDataStd_ExtStringArray myArray;

  /// Stores the label as a field to set array if size is not null (null array is buggy in OCAF)
  TDF_Label myLab;

  friend class Model_Data;
};

#endif
