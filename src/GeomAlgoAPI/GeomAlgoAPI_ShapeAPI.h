// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ShapeAPI.h
// Created:     17 Mar 2016
// Author:      Clarisse Genrault (CEA)

#ifndef GEOMALGOAPI_SHAPEAPI_H
#define GEOMALGOAPI_SHAPEAPI_H

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>

#include <GeomAlgoAPI_Exception.h>

namespace GeomAlgoAPI_ShapeAPI
{
/**\class GeomAlgoAPI_ShapeAPI
 * \ingroup DataAlgo
 * \brief Allows to access the direct API
 */
class GEOMALGOAPI_EXPORT GeomAlgoAPI_ShapeAPI
{
public:
  /// Creates a box using the dimensions.
  /// \param theDx The dimension on X
  /// \param theDy The dimension on Y
  /// \param theDz The dimension on Z
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeBox(const double theDx, const double theDy,
                                                const double theDz) throw (GeomAlgoAPI_Exception);

  /// Creates a box using the two points that defined a diagonal.
  /// \param theFirstPoint One extermity of the diagonal
  /// \param theSecondPoint The other extremity of the diagonal
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeBox(std::shared_ptr<GeomAPI_Pnt> theFirstPoint,
                     std::shared_ptr<GeomAPI_Pnt> theSecondPoint) throw (GeomAlgoAPI_Exception);

  /// Performs a translation from an axis and a distance.
  /// \param theSourceShape Shape to be moved.
  /// \param theAxis Movement axis.
  /// \param theDistance Movement distance.
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeTranslation(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     std::shared_ptr<GeomAPI_Ax1>   theAxis,
                     const double theDistance) throw (GeomAlgoAPI_Exception);

  /// Performs a translation from dimensions.
  /// \param theSourceShape Shape to be moved.
  /// \param theDx Movement dimension on X.
  /// \param theDy Movement dimension on Y.
  /// \param theDz Movement dimension on Z.
  /// \return a shape
  static std::shared_ptr<GeomAPI_Shape> makeTranslation(
                     std::shared_ptr<GeomAPI_Shape> theSourceShape,
                     const double theDx,
                     const double theDy,
                     const double theDz) throw (GeomAlgoAPI_Exception);
};
}
#endif
