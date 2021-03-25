// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
