// Copyright (C) 2019-2022  CEA/DEN, EDF R&D
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

#ifndef GeomDataAPI_Point2DArray_H_
#define GeomDataAPI_Point2DArray_H_

#include <GeomDataAPI.h>
#include <ModelAPI_Attribute.h>

class GeomAPI_Pnt2d;

/**\class GeomDataAPI_Point2DArray
 * \ingroup DataModel
 * \brief Attribute that contains array of 2D point coordinates.
 */

class GeomDataAPI_Point2DArray : public ModelAPI_Attribute
{
public:
  /// Copy values from another array
  /// \return \c true if the copy was successful
  GEOMDATAAPI_EXPORT virtual bool assign(std::shared_ptr<GeomDataAPI_Point2DArray> theOther) = 0;

  /// Returns the size of the array (zero means that it is empty)
  GEOMDATAAPI_EXPORT virtual int size() = 0;

  /// Sets the new size of the array. The previous data is erased.
  GEOMDATAAPI_EXPORT virtual void setSize(const int theSize) = 0;

  /// Defines the value of the array by index [0; size-1]
  GEOMDATAAPI_EXPORT virtual void setPnt(const int theIndex,
                                         const double theX, const double theY) = 0;

  /// Defines the value of the array by index [0; size-1]
  GEOMDATAAPI_EXPORT virtual void setPnt(const int theIndex,
                                         const std::shared_ptr<GeomAPI_Pnt2d>& thePoint) = 0;

  /// Returns the value by the index
  GEOMDATAAPI_EXPORT virtual std::shared_ptr<GeomAPI_Pnt2d> pnt(const int theIndex) = 0;

  /// Returns the type of this class of attributes
  static std::string typeId()
  {
    return std::string("Point2DArray");
  }

  /// Returns the type of this class of attributes, not static method
  GEOMDATAAPI_EXPORT virtual std::string attributeType();

protected:
  /// Objects are created for features automatically
  GEOMDATAAPI_EXPORT GeomDataAPI_Point2DArray();
  GEOMDATAAPI_EXPORT virtual ~GeomDataAPI_Point2DArray();
};

typedef std::shared_ptr<GeomDataAPI_Point2DArray> AttributePoint2DArrayPtr;

#endif
