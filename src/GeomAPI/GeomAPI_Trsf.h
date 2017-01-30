// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAPI_XYZ.hxx
// Created:     13 July 2015
// Author:      Mikhail PONIKAROV
//
// Modified by Clarisse Genrault (CEA) : 17 Nov 2016

#ifndef GeomAPI_Trsf_H_
#define GeomAPI_Trsf_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_Ax1;
class GeomAPI_Ax2;
class GeomAPI_Pnt;

/**\class GeomAPI_Trsf
 * \ingroup DataModel
 * \brief Keep the transformation matrix coefficients
 */

class GeomAPI_Trsf : public GeomAPI_Interface
{
 public:
  /// Keeps no transformation, it may be set by setImpl
  GEOMAPI_EXPORT GeomAPI_Trsf();
  /// Takes the pointer to existing transformation
  GEOMAPI_EXPORT GeomAPI_Trsf(void* theTrsf);

  /** \brief Sets a translation transformation.
   *  \param[in] theAxis     translation axis.
   *  \param[in] theDistance translation distance.
   */
  GEOMAPI_EXPORT void setTranslation(const std::shared_ptr<GeomAPI_Ax1> theAxis,
                                     const double theDistance);

  /** \brief Sets a translation transformation using three coordinates.
   *  \param[in] theDx x coordinate of the translation vector
   *  \param[in] theDy y coordinate of the translation vector
   *  \param[in] theDz z coordinate of the translation vector
   */
  GEOMAPI_EXPORT void setTranslation(const double theDx,
                                     const double theDy,
                                     const double theDz);

  /** \brief Sets a translation transformation using two points.
   *  \param[in] theStartPoint  Start point of the translation vector.
   *  \param[in] theEndPoint    End point of the translation vector.
   */
  GEOMAPI_EXPORT void setTranslation(const std::shared_ptr<GeomAPI_Pnt> theStartPoint,
                                     const std::shared_ptr<GeomAPI_Pnt> theEndPoint);

  /** \brief Sets a rotation transformation.
   *  \param[in] theAxis  rotation axis.
   *  \param[in] theAngle rotation angle(in degree).
   */
  GEOMAPI_EXPORT void setRotation(const std::shared_ptr<GeomAPI_Ax1> theAxis,
                                  const double theAngle);

  /** \brief Sets a point symmetry transformation.
   *  \param[in] thePoint symmetry point.
   */
  GEOMAPI_EXPORT void setSymmetry(const std::shared_ptr<GeomAPI_Pnt> thePoint);

  /** \brief Sets an axis symmetry transformation.
   *  \param[in] theAxis symmetry axis.
   */
  GEOMAPI_EXPORT void setSymmetry(const std::shared_ptr<GeomAPI_Ax1> theAxis);

  /** \brief Sets a plane symmetry transformation.
   *  \param[in] thePlane symmetry plane.
   */
  GEOMAPI_EXPORT void setSymmetry(const std::shared_ptr<GeomAPI_Ax2> thePlane);
  
  /** \brief Sets a scale transformation using a factor.
   *  \param[in] theCenterPoint center point.
   *  \param[in] theScaleFactor scale factor.
   */
  GEOMAPI_EXPORT void setScale(const std::shared_ptr<GeomAPI_Pnt> theCenterPoint,
                               const double theScaleFactor);
};

#endif

