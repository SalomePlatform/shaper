// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeDoubleArray.h
// Created:     08 July 2016
// Author:      Dmitry Bobylev

#ifndef Model_AttributeDoubleArray_H_
#define Model_AttributeDoubleArray_H_

#include "Model.h"

#include <ModelAPI_AttributeDoubleArray.h>

#include <TDF_Label.hxx>
#include <TDataStd_RealArray.hxx>

#include <string>

/// \class Model_AttributeDoubleArray
/// \ingroup DataModel
/// \brief API for the attribute that contains several double in the array inside.
class Model_AttributeDoubleArray: public ModelAPI_AttributeDoubleArray
{
public:
  /// Returns the size of the array (zero means that it is empty)
  MODEL_EXPORT virtual int size();

  /// Sets the new size of the array. The previous data is erased.
  MODEL_EXPORT virtual void setSize(const int theSize);

  /// Defines the value of the array by index [0; size-1]
  MODEL_EXPORT virtual void setValue(const int theIndex,
                                     const double theValue);

  /// Returns the value by the index
  MODEL_EXPORT virtual double value(const int theIndex);

protected:
  /// Initializes attibutes
  Model_AttributeDoubleArray(TDF_Label& theLabel);
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

private:
  /// The OCCT array that keeps all values.
  Handle_TDataStd_RealArray myArray;

  /// Stores the label as a field to set array if size is not null (null array is buggy in OCAF)
  TDF_Label myLab;

  friend class Model_Data;
};

#endif
