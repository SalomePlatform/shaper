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
  MODEL_EXPORT virtual void setSize(const int theSize,
                                    bool sendUpdated = true);

  /// Defines the value of the array by index [0; size-1]
  MODEL_EXPORT virtual void setValue(const int theIndex,
                                     const int theValue,
                                     bool sendUpdated = true);

  /// Returns the value by the index
  MODEL_EXPORT virtual int value(const int theIndex);


 protected:
  /// Initializes attibutes
  Model_AttributeIntArray(TDF_Label& theLabel);
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  friend class Model_Data;
};

#endif
