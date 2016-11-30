// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Box.h
// Created:     17 Mar 2016
// Author:      Clarisse Genrault (CEA)

#ifndef GEOMALGOAPI_BOX_H_
#define GEOMALGOAPI_BOX_H_

#include <GeomAPI_Pnt.h>
#include <GeomAlgoAPI_MakeShape.h>

/**\class GeomAlgoAPI_Box
 * \ingroup DataAlgo
 * \brief Allows to create Box Primitives
 */
class GeomAlgoAPI_Box : public GeomAlgoAPI_MakeShape
{
 public:
  /// Type of box operation
  enum MethodType {
    BOX_DIM,   ///< Box with dimensions
    BOX_POINTS,  ///< Box with points
  };

  GEOMALGOAPI_EXPORT GeomAlgoAPI_Box();

  /// Creates a box using the dimensions.
  /// \param theDx The dimension on X
  /// \param theDy The dimension on Y
  /// \param theDz The dimension on Z
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Box(const double theDx, const double theDy, const double theDz);

  /// Creates a box using the two points that defined a diagonal.
  /// \param theFirstPoint One extermity of the diagonal
  /// \param theSecondPoint The other extremity of the diagonal
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Box(std::shared_ptr<GeomAPI_Pnt> theFirstPoint,
                                     std::shared_ptr<GeomAPI_Pnt> theSecondPoint);

  /// Checks if data for the box construction is OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Builds the box.
  GEOMALGOAPI_EXPORT void build();

  /// Prepare the naming (redifined because it is specific for a box).
  GEOMALGOAPI_EXPORT void prepareNamingFaces();

 private:
  /// Builds the box with the dimensions "Dx", "Dy" and "Dz".
  void buildWithDimensions();
  /// Builds the box with two points
  void buildWithPoints();

  double myDx; /// Dimension on X to create a box.
  double myDy; /// Dimension on Y to create a box.
  double myDz; /// Dimension Z to create a box.
  std::shared_ptr<GeomAPI_Pnt> myFirstPoint; /// First point to create a box.
  std::shared_ptr<GeomAPI_Pnt> mySecondPoint; /// Second point to create a box.
  MethodType myMethodType; /// Type of method used.
};


#endif // GEOMALGOAPI_BOX_H_
