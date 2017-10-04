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

#ifndef GeomAPI_Curve_H_
#define GeomAPI_Curve_H_

#include <GeomAPI_Shape.h>
#include <memory>

class GeomAPI_Pnt;

/**\class GeomAPI_Curve
 * \ingroup DataModel
 * \brief Interface to the generic curve object
 */

class GeomAPI_Curve : public GeomAPI_Interface
{
 public:
  /// Creation of empty (null) shape
  GEOMAPI_EXPORT
  GeomAPI_Curve();

  /// Creates a curve from the shape (edge)
  GEOMAPI_EXPORT
  GeomAPI_Curve(const std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Returns true if curve is not initialized
  GEOMAPI_EXPORT
  bool isNull() const;

  /// Returns whether the curve is linear
  GEOMAPI_EXPORT
  virtual bool isLine() const;

  /// Returns whether the curve is circular
  GEOMAPI_EXPORT
  virtual bool isCircle() const;

  /// Returns start parameter of the curve
  GEOMAPI_EXPORT
  double startParam() const { return myStart; }

  /// Returns end parameter of the curve
  GEOMAPI_EXPORT
  double endParam() const { return myEnd; }

  /// Returns point on the curve by parameter
  /// \param theParam parameter on the curve
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt> getPoint(double theParam);

private:
  double myStart;
  double myEnd;
};

//! Pointer on the object
typedef std::shared_ptr<GeomAPI_Curve> GeomCurvePtr;

#endif
