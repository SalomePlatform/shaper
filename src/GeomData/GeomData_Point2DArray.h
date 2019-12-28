// Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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

#ifndef GeomData_Point2DArray_H_
#define GeomData_Point2DArray_H_

#include "GeomData.h"

#include <GeomDataAPI_Point2DArray.h>

#include <TDataStd_RealArray.hxx>
#include <TDF_Label.hxx>

/** \class GeomData_Point2DArray
 *  \ingroup DataModel
 *  \brief Attribute that contains an array of 2D points.
 */
class GeomData_Point2DArray : public GeomDataAPI_Point2DArray
{
  TDF_Label myLab; ///< the main label of the attribute
  Handle_TDataStd_RealArray myArray; ///< array that keeps all coordinates of the points

public:
  /// Copy values from another array
  /// \return \c true if the copy was successful
  GEOMDATA_EXPORT virtual bool assign(std::shared_ptr<GeomDataAPI_Point2DArray> theOther);

  /// Returns the size of the array (zero means that it is empty)
  GEOMDATA_EXPORT virtual int size();

  /// Sets the new size of the array. The previous data is erased.
  GEOMDATA_EXPORT virtual void setSize(const int theSize);

  /// Defines the value of the array by index [0; size-1]
  GEOMDATA_EXPORT virtual void setPnt(const int theIndex,
                                      const double theX, const double theY);

  /// Defines the value of the array by index [0; size-1]
  GEOMDATA_EXPORT virtual void setPnt(const int theIndex,
                                      const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Returns the value by the index
  GEOMDATA_EXPORT virtual std::shared_ptr<GeomAPI_Pnt2d> pnt(const int theIndex);

protected:
  /// Initializes attributes
  GEOMDATA_EXPORT GeomData_Point2DArray(TDF_Label& theLabel);
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  friend class Model_Data;
};

#endif
