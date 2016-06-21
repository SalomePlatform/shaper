// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_BoxPoints.h
// Created:     17 Mar 2016
// Author:      Clarisse Genrault (CEA)

#ifndef GeomAlgoAPI_BoxPoints_H_
#define GeomAlgoAPI_BoxPoints_H_

#include <GeomAPI_Pnt.h>
#include <GeomAlgoAPI_Box.h>

/**\class GeomAlgoAPI_BoxPoints
 * \ingroup DataAlgo
 * \brief Allows to create Box Primitives using the two points that defined a diagonal.
 */
class GeomAlgoAPI_BoxPoints : public GeomAlgoAPI_Box
{
 public: 
  /// Creates a box using the two points that defined a diagonal.
  /// \param theFirstPoint One extermity of the diagonal
  /// \param theSecondPoint The other extremity of the diagonal
  GEOMALGOAPI_EXPORT GeomAlgoAPI_BoxPoints(std::shared_ptr<GeomAPI_Pnt> theFirstPoint,
                                           std::shared_ptr<GeomAPI_Pnt> theSecondPoint);
  
  /// \return true if the data of the construction of the box were correct.
  GEOMALGOAPI_EXPORT bool check();
  
  /// Builds the box.
  GEOMALGOAPI_EXPORT void build();
  
 private:
  std::shared_ptr<GeomAPI_Pnt> myFirstPoint; /// First point to create a box.
  std::shared_ptr<GeomAPI_Pnt> mySecondPoint; /// Second point to create a box.
};


#endif
