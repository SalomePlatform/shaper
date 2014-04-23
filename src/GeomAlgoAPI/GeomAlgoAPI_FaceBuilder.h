// File:        GeomAlgoAPI_FaceBuilder.h
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAlgoAPI_FaceBuilder_HeaderFile
#define GeomAlgoAPI_FaceBuilder_HeaderFile

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>
#include <memory>

/**\class GeomAlgoAPI_FaceBuilder
 * \ingroup DataAlgo
 * \brief Allows to create face-shapes by different parameters
 */

class GEOMALGOAPI_EXPORT GeomAlgoAPI_FaceBuilder
{
public:
  /// Creates square planar face by given point of the center,
  /// normal to the plane and size of square
  static std::shared_ptr<GeomAPI_Shape> square(std::shared_ptr<GeomAPI_Pnt> theCenter,
    std::shared_ptr<GeomAPI_Dir> theNormal, const double theSize);
};

#endif
