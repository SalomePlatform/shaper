// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef GEOMALGOAPI_SCALE_H_
#define GEOMALGOAPI_SCALE_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_Transform.h>

class GeomAPI_Pnt;

/// \class GeomAlgoAPI_Scale
/// \ingroup DataAlgo
/// \brief Creates a copy of the object by performing a scale operation by a factor or
///        by dimensions.
class GeomAlgoAPI_Scale : public GeomAlgoAPI_Transform
{
public:
  /// \brief Creates an object which is obtained from current object by performing
  ///        a scale operation by a factor.
  /// \param[in] theSourceShape  the shape to be moved.
  /// \param[in] theCenterPoint  the center point.
  /// \param[in] theFactor       the scale factor.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Scale(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                       std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
                                       double                         theScaleFactor);

  /// \brief Creates an object which is obtained from current object by performing
  ///        a scale operation by dimensions.
  /// \param[in] theSourceShape  the shape to be moved.
  /// \param[in] theCenterPoint  the center point.
  /// \param[in] theFactorX      the scale factor in X.
  /// \param[in] theFactorY      the scale factor in Y.
  /// \param[in] theFactorZ      the scale factor in Z.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Scale(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                       std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
                                       double                         theScaleFactorX,
                                       double                         theScaleFactorY,
                                       double                         theScaleFactorZ);

private:
  void buildByDimensions(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                         std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
                         double                         theScaleFactorX,
                         double                         theScaleFactorY,
                         double                         theScaleFactorZ);
};

#endif // GEOMALGOAPI_SCALE_H_