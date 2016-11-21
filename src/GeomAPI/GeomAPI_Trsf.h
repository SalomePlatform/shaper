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

  /** \brief Sets a rotation transformation.
   *  \param[in] theAxis  rotation axis.
   *  \param[in] theAngle rotation angle(in degree).
   */
  GEOMAPI_EXPORT void setRotation(const std::shared_ptr<GeomAPI_Ax1> theAxis,
                                  const double theAngle);
};

#endif

