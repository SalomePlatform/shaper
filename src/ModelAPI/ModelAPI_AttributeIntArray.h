// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef ModelAPI_AttributeIntArray_H_
#define ModelAPI_AttributeIntArray_H_

#include <ModelAPI.h>
#include <ModelAPI_Attribute.h>

#include <string>


/**\class ModelAPI_AttributeIntArray
 * \ingroup DataModel
 * \brief API for the attribute that contains several integers in the array inside.
 * Used for RGB color storage for an example. By default size is one, zero-based.
 */

class ModelAPI_AttributeIntArray : public ModelAPI_Attribute
{
 public:

  /// Returns the size of the array (zero means that it is empty)
  MODELAPI_EXPORT virtual int size() = 0;

  /// Sets the new size of the array. The previous data is erased.
  MODELAPI_EXPORT virtual void setSize(const int theSize) = 0;

  /// Defines the value of the array by index [0; size-1]
  MODELAPI_EXPORT virtual void setValue(const int theIndex,
                                        const int theValue,
                                        bool sendUpdated = true) = 0;

  /// Returns the value by the index
  MODELAPI_EXPORT virtual int value(const int theIndex) = 0;

  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string typeId()
  {
    return "IntArray";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType();

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeIntArray();

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeIntArray();
};

//! Pointer on double attribute
typedef std::shared_ptr<ModelAPI_AttributeIntArray> AttributeIntArrayPtr;

#endif
