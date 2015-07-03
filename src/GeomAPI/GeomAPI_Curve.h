// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Curve.hxx
// Created:     04 Sep 2014
// Author:      Mikhail PONIKAROV

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

#endif
