// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Box.h
// Created:     17 Mar 2016
// Author:      Clarisse Genrault (CEA)

#ifndef GeomAlgoAPI_Box_H_
#define GeomAlgoAPI_Box_H_

#include <GeomAPI_Pnt.h>
#include <GeomAlgoAPI_MakeShape.h>

/**\class GeomAlgoAPI_Box
 * \ingroup DataAlgo
 * \brief Allows to create Box Primitives
 */
class GeomAlgoAPI_Box : public GeomAlgoAPI_MakeShape
{
 public: 
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Box();
  
  /// Creates a box using the dimensions.
  /// \param theDx The dimension on X
  /// \param theDy The dimension on Y
  /// \param theDz The dimension on Z
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Box(const double theDx, const double theDy, const double theDz);
  
  /// Checks if each dimension "Dx", Dy" and "Dz" for the box construction is OK.
  GEOMALGOAPI_EXPORT bool check();
  
  /// Builds the box with the dimensions "Dx", "Dy" and "Dz".
  GEOMALGOAPI_EXPORT void build();
  
  /// Prepare the naming (redifined because it is specific for a box).
  GEOMALGOAPI_EXPORT void prepareNamingFaces();
  
 private:
  double myDx; /// Dimension on X to create a box.
  double myDy; /// Dimension on Y to create a box.
  double myDz; /// Dimension Z to create a box.
};


#endif
