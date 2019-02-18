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

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_REFATTR_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_REFATTR_H_

//--------------------------------------------------------------------------------------
#include "ModelHighAPI.h"

#include <memory>
#include <string>
//--------------------------------------------------------------------------------------
class ModelAPI_Attribute;
class ModelAPI_AttributeRefAttr;
class ModelAPI_AttributeRefAttrList;
class ModelAPI_Object;
class ModelAPI_ObjectMovedMessage;
class ModelHighAPI_Interface;
//--------------------------------------------------------------------------------------
/**\class ModelHighAPI_RefAttr
 * \ingroup CPPHighAPI
 * \brief Class for filling ModelAPI_AttributeRefAttr
 */
class ModelHighAPI_RefAttr
{
public:
  /// Default constructor
  MODELHIGHAPI_EXPORT
  ModelHighAPI_RefAttr();
  /// Constructor for attribute
  MODELHIGHAPI_EXPORT
  ModelHighAPI_RefAttr(const std::shared_ptr<ModelAPI_Attribute> & theValue);
  /// Constructor for object
  MODELHIGHAPI_EXPORT
  ModelHighAPI_RefAttr(const std::shared_ptr<ModelAPI_Object> & theValue);
  /// Constructor for Interface (use result() for object)
  MODELHIGHAPI_EXPORT
  ModelHighAPI_RefAttr(const std::shared_ptr<ModelHighAPI_Interface> & theValue);
  /// Destructor
  MODELHIGHAPI_EXPORT
  virtual ~ModelHighAPI_RefAttr();

  /// Fill attribute values
  MODELHIGHAPI_EXPORT virtual
    void fillAttribute(const std::shared_ptr<ModelAPI_AttributeRefAttr> & theAttribute) const;

  /// Append to list attribute
  MODELHIGHAPI_EXPORT virtual
    void appendToList(const std::shared_ptr<ModelAPI_AttributeRefAttrList> & theAttribute) const;

  /// Check the object is empty
  MODELHIGHAPI_EXPORT
  bool isEmpty() const;

  /// Fill moved message by the attribute or object
  MODELHIGHAPI_EXPORT
  void fillMessage(const std::shared_ptr<ModelAPI_ObjectMovedMessage>& theMessage) const;

private:
  enum VariantType { VT_ATTRIBUTE, VT_OBJECT } myVariantType;
  std::shared_ptr<ModelAPI_Attribute> myAttribute;
  std::shared_ptr<ModelAPI_Object> myObject;
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_REFATTR_H_ */
