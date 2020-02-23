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

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_INTEGER_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_INTEGER_H_

//--------------------------------------------------------------------------------------
#include "ModelHighAPI.h"

#include <memory>
#include <string>
//--------------------------------------------------------------------------------------
class ModelAPI_AttributeInteger;
//--------------------------------------------------------------------------------------
/**\class ModelHighAPI_Integer
 * \ingroup CPPHighAPI
 * \brief Class for filling ModelAPI_AttributeInteger
 */
class ModelHighAPI_Integer
{
public:
  /// Constructor for int
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Integer(int theValue = 0);
  /// Constructor for std::string
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Integer(const std::string & theValue);
  /// Constructor for char *
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Integer(const char * theValue);
  /// Destructor
  MODELHIGHAPI_EXPORT
  virtual ~ModelHighAPI_Integer();

  /// Fill attribute values
  MODELHIGHAPI_EXPORT
  virtual void fillAttribute(const std::shared_ptr<ModelAPI_AttributeInteger> & theAttribute) const;

  /// Returns a value (must be used only for attributes which support no text)
  MODELHIGHAPI_EXPORT virtual int intValue() const;

  /// Returns a string representation of the value
  MODELHIGHAPI_EXPORT virtual std::string string() const;

private:
  enum VariantType { VT_INT, VT_STRING } myVariantType;
  int myInt;
  std::string myString;
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_INTEGER_H_ */
