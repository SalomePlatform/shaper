// Copyright (C) 2017-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Ellipse2d.h
// Created:     26 April 2017
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Ellipse2d_H_
#define GeomAPI_Ellipse2d_H_

#include <GeomAPI_Interface.h>

class GeomAPI_Pnt2d;
class GeomAPI_Dir2d;

/**\class GeomAPI_Ellipse2d
 * \ingroup DataModel
 * \brief Ellipse in 2D
 */
class GeomAPI_Ellipse2d : public GeomAPI_Interface
{
public:
  /// \brief Constructs ellipse by center, X-axis and given radii
  GEOMAPI_EXPORT GeomAPI_Ellipse2d(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                   const std::shared_ptr<GeomAPI_Dir2d>& theXAxis,
                                   const double theMajorRadius,
                                   const double theMinorRadius);

  /// \brief Constructs ellipse by center and two points lying on the ellipse:
  ///        first of them defines an axis of the ellipse
  ///        and another is just placed on the ellipse.
  GEOMAPI_EXPORT GeomAPI_Ellipse2d(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                   const std::shared_ptr<GeomAPI_Pnt2d>& theAxisPoint,
                                   const std::shared_ptr<GeomAPI_Pnt2d>& thePassingPoint);

  /// Returns center of the ellipse
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt2d> center() const;

  /// Returns first focus of the ellipse
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt2d> firstFocus() const;

  /// Returns second focus of the ellipse
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt2d> secondFocus() const;

  /// Returns minor radius of the ellipse
  GEOMAPI_EXPORT double minorRadius() const;

  /// Returns major radius of the ellipse
  GEOMAPI_EXPORT double majorRadius() const;
};

#endif
