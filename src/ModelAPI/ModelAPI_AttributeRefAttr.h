// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef ModelAPI_AttributeRefAttr_H_
#define ModelAPI_AttributeRefAttr_H_

#include "ModelAPI_Attribute.h"
#include "ModelAPI_Feature.h"

/**\class ModelAPI_AttributeRefAttr
 * \ingroup DataModel
 * \brief Attribute that contains reference to an attribute of a feature or reference to 
 * a feature (switchable)
 */

class ModelAPI_AttributeRefAttr : public ModelAPI_Attribute
{
 public:
  /// Returns true if this attribute references to a object (not to the attribute)
  MODELAPI_EXPORT virtual bool isObject() = 0;

  /// Defines the reference to the attribute
  MODELAPI_EXPORT virtual void setAttr(AttributePtr theAttr) = 0;

  /// Returns attribute referenced from this attribute
  MODELAPI_EXPORT virtual AttributePtr attr() = 0;

  /// Defines the reference to the object
  MODELAPI_EXPORT virtual void setObject(ObjectPtr theFeature) = 0;

  /// Returns object referenced from this attribute
  MODELAPI_EXPORT virtual ObjectPtr object() = 0;

  /// Returns the type of this class of attributes
  MODELAPI_EXPORT static std::string typeId()
  {
    return "RefAttr";
  }

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType();

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeRefAttr();

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_AttributeRefAttr();
};

typedef std::shared_ptr<ModelAPI_AttributeRefAttr> AttributeRefAttrPtr;

#endif
