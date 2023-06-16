// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#ifndef GeomDataAPI_Point_H_
#define GeomDataAPI_Point_H_

#include "GeomDataAPI.h"
#include <ModelAPI_Attribute.h>

#include <set>

class GeomAPI_Pnt;

/**\class GeomDataAPI_Point
 * \ingroup DataModel
 * \brief Attribute that contains 3D point coordinates. 
 */

class GeomDataAPI_Point : public ModelAPI_Attribute
{
 public:
  /// Defines the double value
  GEOMDATAAPI_EXPORT virtual
    void setValue(const double theX, const double theY, const double theZ) = 0;
  /// Defines the point
  GEOMDATAAPI_EXPORT virtual void setValue(const std::shared_ptr<GeomAPI_Pnt>& thePoint) = 0;

  /// Returns the X double value
  GEOMDATAAPI_EXPORT virtual double x() const = 0;
  /// Returns the Y double value
  GEOMDATAAPI_EXPORT virtual double y() const = 0;
  /// Returns the Z double value
  GEOMDATAAPI_EXPORT virtual double z() const = 0;
  /// Returns the 3D point
  GEOMDATAAPI_EXPORT virtual std::shared_ptr<GeomAPI_Pnt> pnt() = 0;

  /// Defines the calculated double value
  GEOMDATAAPI_EXPORT virtual
    void setCalculatedValue(const double theX, const double theY, const double theZ) = 0;

  /// Defines the X coordinate value
  GEOMDATAAPI_EXPORT virtual void setX(const double theX) = 0;
  /// Defines the Y coordinate value
  GEOMDATAAPI_EXPORT virtual void setY(const double theY) = 0;
  /// Defines the Z coordinate value
  GEOMDATAAPI_EXPORT virtual void setZ(const double theZ) = 0;

  /// Defines the text values
  GEOMDATAAPI_EXPORT virtual void setText(const std::wstring& theX,
                                          const std::wstring& theY,
                                          const std::wstring& theZ) = 0;

  /// Defines the X text value
  GEOMDATAAPI_EXPORT virtual void setTextX(const std::wstring& theX) = 0;
  /// Defines the Y text value
  GEOMDATAAPI_EXPORT virtual void setTextY(const std::wstring& theY) = 0;
  /// Defines the Z text value
  GEOMDATAAPI_EXPORT virtual void setTextZ(const std::wstring& theZ) = 0;

  /// Returns the text value for X
  GEOMDATAAPI_EXPORT virtual std::wstring textX() = 0;
  /// Returns the text value for Y
  GEOMDATAAPI_EXPORT virtual std::wstring textY() = 0;
  /// Returns the text value for Z
  GEOMDATAAPI_EXPORT virtual std::wstring textZ() = 0;

  /// Point component (x,y,z)
  enum PointComponent { C_X = 0,
                        C_Y = 1,
                        C_Z = 2,
  };

  /// Allows to set expression (text) as invalid (by the parameters listener)
  GEOMDATAAPI_EXPORT virtual void setExpressionInvalid(int theComponent, const bool theFlag) = 0;

  /// Returns true if text is invalid
  GEOMDATAAPI_EXPORT virtual bool expressionInvalid(int theComponent) = 0;

  /// Allows to set expression (text) error (by the parameters listener)
  GEOMDATAAPI_EXPORT virtual
    void setExpressionError(int theComponent, const std::string& theError) = 0;

  /// Returns an expression error
  GEOMDATAAPI_EXPORT virtual std::string expressionError(int theComponent) = 0;

  /// Defines the used parameters
  GEOMDATAAPI_EXPORT virtual void setUsedParameters(int theComponent,
    const std::set<std::wstring>& theUsedParameters) = 0;

  /// Returns the used parameters
  GEOMDATAAPI_EXPORT virtual std::set<std::wstring> usedParameters(int theComponent) const = 0;

  /// Returns the type of this class of attributes
  static std::string typeId()
  {
    return std::string("Point");
  }

  /// Returns the type of this class of attributes, not static method
  GEOMDATAAPI_EXPORT virtual std::string attributeType();

 protected:
  /// Objects are created for features automatically
  GEOMDATAAPI_EXPORT GeomDataAPI_Point();
  GEOMDATAAPI_EXPORT virtual ~GeomDataAPI_Point();
};

//! Pointer on attribute object
typedef std::shared_ptr<GeomDataAPI_Point> AttributePointPtr;

#endif
