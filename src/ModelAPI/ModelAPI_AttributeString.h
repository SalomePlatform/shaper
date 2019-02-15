// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef MODELAPI_ATTRIBUTESTRING_H_
#define MODELAPI_ATTRIBUTESTRING_H_

#include "ModelAPI_Attribute.h"

#include <string>

/**\class ModelAPI_AttributeString
 * \ingroup DataModel
 * \brief API for the attribute that contains std (null terminated) string.
 */

class ModelAPI_AttributeString : public ModelAPI_Attribute
{
 public:
  /// Defines the string value
  MODELAPI_EXPORT virtual void setValue(const std::string& theValue) = 0;
  /// Defines the wstring value
  MODELAPI_EXPORT virtual void setValue(const std::wstring& theValue) = 0;

  /// Returns the string value
  MODELAPI_EXPORT virtual std::string value() = 0;
  /// Returns the wstring value
  MODELAPI_EXPORT virtual std::wstring valueW() = 0;

  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string typeId()
  {
    return "String";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType();

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeString();

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeString();
};

//! Pointer on string attribute
typedef std::shared_ptr<ModelAPI_AttributeString> AttributeStringPtr;

#endif
