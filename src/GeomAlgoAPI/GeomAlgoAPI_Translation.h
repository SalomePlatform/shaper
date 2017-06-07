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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef GeomAlgoAPI_Translation_H_
#define GeomAlgoAPI_Translation_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Translation
/// \ingroup DataAlgo
/// \brief Creates a copy of the object by moving it along the axis.
class GeomAlgoAPI_Translation : public GeomAlgoAPI_MakeShape
{
public:
  /// Type of translation operation
  enum MethodType {
    BY_DISTANCE, ///< Translation by axis and distance
    BY_DIM,      ///< Translation by dimensions in X, Y and Z
    BY_POINTS    ///< Translation by two points
  };

  /// \brief Creates an object which is obtained from current object by moving it along the axis.
  /// \param[in] theSourceShape  a shape to be moved.
  /// \param[in] theAxis         movement axis.
  /// \param[in] theDistance     movement distance.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Translation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                             std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                             double                         theDistance);

  /// \brief Creates an object which is obtained from current object by moving it along a vector
  ///        defined by its dimensions in X, Y and Z.
  /// \param[in] theSourceShape  the shape to be moved.
  /// \param[in] theDX           the movement dimension in X.
  /// \param[in] theDY           the movement dimension in Y.
  /// \param[in] theDZ           the movement dimension in Z.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Translation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                             double                         theDx,
                                             double                         theDy,
                                             double                         theDz);

  /// \brief Creates an object which is obtained from current object by moving it along a vector
  ///        defined by two points.
  /// \param[in] theSourceShape  the shape to be moved.
  /// \param[in] theStartPoint   the movement start point.
  /// \param[in] theEndPoint     the movement end point.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Translation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                             std::shared_ptr<GeomAPI_Pnt>   theStartPoint,
                                             std::shared_ptr<GeomAPI_Pnt>   theEndPoint);

  /// Checks if data for the translation execution is OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Execute the translation.
  GEOMALGOAPI_EXPORT void build();

private:
  MethodType myMethodType; /// Type of method used.
  std::shared_ptr<GeomAPI_Shape> mySourceShape; /// Shape to be moved.
  std::shared_ptr<GeomAPI_Ax1> myAxis; /// Movement axis.
  double myDistance; /// Movement distance.
  double myDx; /// Movement dimension on X.
  double myDy; /// Movement dimension on Y.
  double myDz; /// Movement dimension on Z.
  std::shared_ptr<GeomAPI_Pnt> myStartPoint; /// Movement start point.
  std::shared_ptr<GeomAPI_Pnt> myEndPoint; /// Movement end point.
};

#endif
