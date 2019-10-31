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

  /** \brief Sets a rotation transformation using three points.
   *  \param[in] theCenterPoint  rotation center.
   *  \param[in] theStartPoint   start rotation point.
   *  \param[in] theEndPoint     end rotation point.
   */
  GEOMAPI_EXPORT void setRotation(const std::shared_ptr<GeomAPI_Pnt> theCenterPoint,
                                  const std::shared_ptr<GeomAPI_Pnt> theStartPoint,
                                  const std::shared_ptr<GeomAPI_Pnt> theEndPoint);

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
};

//! Pointer on the object
typedef std::shared_ptr<GeomAPI_Trsf> GeomTrsfPtr;

#endif

