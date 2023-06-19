// Copyright (C) 2014-2023  CEA, EDF
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

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_DOUBLE_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_DOUBLE_H_

//--------------------------------------------------------------------------------------
#include "ModelHighAPI.h"

#include <memory>
#include <string>
//--------------------------------------------------------------------------------------
class ModelAPI_AttributeDouble;
class GeomDataAPI_Point;
//--------------------------------------------------------------------------------------
/**\class ModelHighAPI_Double
 * \ingroup CPPHighAPI
 * \brief Class for filling ModelAPI_AttributeDouble
 */
class ModelHighAPI_Double
{
public:
  /// Constructor for double
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Double(double theValue = 0.);
  /// Constructor for std::string
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Double(const std::wstring & theValue);
  /// Constructor for char *
  MODELHIGHAPI_EXPORT
  ModelHighAPI_Double(const wchar_t * theValue);
  /// Destructor
  MODELHIGHAPI_EXPORT
  virtual ~ModelHighAPI_Double();

  /// Fill attribute values
  MODELHIGHAPI_EXPORT
  virtual void fillAttribute(const std::shared_ptr<ModelAPI_AttributeDouble> & theAttribute) const;

  /// Sets the zero-based coordinates of a point
  MODELHIGHAPI_EXPORT virtual void fillAttribute(
    const std::shared_ptr<GeomDataAPI_Point> & thePoint,
    const ModelHighAPI_Double & theX,
    const ModelHighAPI_Double & theY,
    const ModelHighAPI_Double & theZ) const;

  /// Value of the attribute
  MODELHIGHAPI_EXPORT double value() const;

  /// Returns a string representation of the value
  MODELHIGHAPI_EXPORT virtual std::wstring string() const;

private:
  enum VariantType { VT_DOUBLE, VT_STRING } myVariantType;
  double myDouble;
  std::wstring myString;
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_DOUBLE_H_ */
