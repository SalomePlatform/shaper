// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Ellipse.h
// Created:     25 April 2017
// Author:      Vitaly Smetannikov

#ifndef GeomAPI_Ellipse_H_
#define GeomAPI_Ellipse_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_Pnt;
class GeomAPI_Ax2;


/**\class GeomAPI_Ellipse
 * \ingroup DataModel
 * \brief Ellipse in 3D
 */
class GeomAPI_Ellipse : public GeomAPI_Interface
{
public:

  /// \brief Constructs an epty ellipse
  GEOMAPI_EXPORT GeomAPI_Ellipse() : GeomAPI_Interface() {}

  /** \brief Constructs an ellipse with major and minor radiuses,
   *  where theAx2 locates the ellipse and defines its orientation in 3D space such that:\n
   *  - the center of the circle is the origin of theAx2;\n
   *  - the origin, "X Direction" and "Y Direction" of theAx2 define the plane of the circle;\n
   *  - theAx2 is the local coordinate system of the circle.\n
   *    Note: It is possible to create a circle where Radius is equal to 0.0. raised if Radius < 0.
   */
  GEOMAPI_EXPORT GeomAPI_Ellipse(const std::shared_ptr<GeomAPI_Ax2>& theAx2,
                                 double theMajorRadius, double theMinorRadius);

  /// Returns first center of the ellipse
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt> firstFocus() const;

  /// Returns second center of the ellipse
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt> secondFocus() const;

  /// Returns minor radius of the ellipse
  GEOMAPI_EXPORT double minorRadius() const;

  /// Returns major radius of the ellipse
  GEOMAPI_EXPORT double majorRadius() const;

};

//! Pointer on the object
typedef std::shared_ptr<GeomAPI_Ellipse> GeomEllipsePtr;

#endif
