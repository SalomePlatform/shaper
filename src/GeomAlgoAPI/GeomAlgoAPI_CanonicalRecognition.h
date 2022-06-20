// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef GeomAlgoAPI_CanonicalRecognition_H_
#define GeomAlgoAPI_CanonicalRecognition_H_

#include "GeomAlgoAPI.h"
#include <GeomAPI_Shape.h>

#include <vector>

class GeomAlgoAPI_CanonicalRecognition
{
public:
  /*!
   *  \brief Check if the shape is planar
   */
  GEOMALGOAPI_EXPORT static bool isPlane(const GeomShapePtr& theShape, double theTolerance,
    std::vector<double>& theNormal, std::vector<double>& theOrigin);

  /*!
   * \brief Check if shape is spherical
   */
  GEOMALGOAPI_EXPORT static bool isSphere(const GeomShapePtr& theShape, double theTolerance,
    std::vector<double>& theOrigin, double& theRadius);

  /*!
   * \brief Check if shape is conical
   */
  GEOMALGOAPI_EXPORT static bool isCone(const GeomShapePtr& theShape, double theTolerance,
    std::vector<double>& theAxis, std::vector<double>& theApex,
    double& theHalfAngle);

  /*!
   * \brief Check if shape is cylinder
   */
  GEOMALGOAPI_EXPORT static bool isCylinder(const GeomShapePtr& theShape, double theTolerance,
    std::vector<double>& theAxis, std::vector<double>& theOrigin,
    double& theRadius);

  /*!
   * \brief Check if edge / wire is line
   */
  GEOMALGOAPI_EXPORT static bool isLine(const GeomShapePtr& theEdge, double theTolerance,
    std::vector<double>& theDir, std::vector<double>& theOrigin);

  /*!
   * \brief Check if edge / wire is circle
   */
  GEOMALGOAPI_EXPORT static bool isCircle(const GeomShapePtr& theEdge, double theTolerance,
    std::vector<double>& theNormal, std::vector<double>& theOrigin,
    double& theRadius);

  /*!
   * \brief Check if edge / wire is ellipse
   */
  GEOMALGOAPI_EXPORT static bool isEllipse(const GeomShapePtr& theEdge, double theTolerance,
    std::vector<double>& theNormal, std::vector<double>& theDirX,
    std::vector<double>& theOrigin,
    double& theMajorRadius, double& theMinorRadius);

  /*!
   * \brief Check if the algorithm is implemented (Shaper is built with appropriate OCCT version)
   */
  GEOMALGOAPI_EXPORT static bool isImplemented();

};

#endif
