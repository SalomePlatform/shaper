// File:        GeomAlgoAPI_PointBuilder.h
// Created:     02 Jun 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAlgoAPI_PointBuilder_H_
#define GeomAlgoAPI_PointBuilder_H_

#include <GeomAlgoAPI.h>
#include <memory>

class GeomAPI_Shape;
class GeomAPI_Pnt;

/**\class GeomAlgoAPI_PointBuilder
 * \ingroup DataAlgo
 * \brief Allows to create face-shapes by different parameters
 */

class GEOMALGOAPI_EXPORT GeomAlgoAPI_PointBuilder
{
 public:
  /// Creates linear edge by two points
  static std::shared_ptr<GeomAPI_Shape> point(std::shared_ptr<GeomAPI_Pnt> thePoint);
};

#endif
