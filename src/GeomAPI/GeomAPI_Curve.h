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

  /// Returns \c true if curves are equal
  GEOMAPI_EXPORT
  bool isEqual(const std::shared_ptr<GeomAPI_Curve>& theOther) const;

  /// Returns whether the curve is linear
  GEOMAPI_EXPORT
  virtual bool isLine() const;

  /// Returns whether the curve is circular
  GEOMAPI_EXPORT
  virtual bool isCircle() const;

  /// Returns whether the curve is elliptic
  GEOMAPI_EXPORT
  virtual bool isEllipse() const;

  /// Returns start parameter of the curve
  GEOMAPI_EXPORT
  double startParam() const { return myStart; }

  /// Returns end parameter of the curve
  GEOMAPI_EXPORT
  double endParam() const { return myEnd; }

  /// Returns \c true if the curve is trimmed
  GEOMAPI_EXPORT
  virtual bool isTrimmed() const;

  /// Returns basis for the trimmed curve
  GEOMAPI_EXPORT
  virtual std::shared_ptr<GeomAPI_Curve> basisCurve() const;

  /// Returns point on the curve by parameter
  /// \param theParam parameter on the curve
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt> getPoint(double theParam);

public:
  /// \brief Compare addresses of curves
  class Comparator
  {
  public:
    /// Return \c true if the address of the first curve is less than the address of the second
    GEOMAPI_EXPORT
    bool operator ()(const std::shared_ptr<GeomAPI_Curve>& theCurve1,
                     const std::shared_ptr<GeomAPI_Curve>& theCurve2) const;
  };

private:
  double myStart;
  double myEnd;
};

//! Pointer on the object
typedef std::shared_ptr<GeomAPI_Curve> GeomCurvePtr;

#endif
