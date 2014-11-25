// File:        GeomAlgoAPI_EdgeBuilder.h
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAlgoAPI_EdgeBuilder_H_
#define GeomAlgoAPI_EdgeBuilder_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>
#include <memory>

/**\class GeomAlgoAPI_EdgeBuilder
 * \ingroup DataAlgo
 * \brief Allows to create face-shapes by different parameters
 */

class GEOMALGOAPI_EXPORT GeomAlgoAPI_EdgeBuilder
{
 public:
  /// Creates linear edge by two points
  static std::shared_ptr<GeomAPI_Edge> line(std::shared_ptr<GeomAPI_Pnt> theStart,
                                              std::shared_ptr<GeomAPI_Pnt> theEnd);

  /// Creates linear edge in a form of a circle by a point and a circle radius
  static std::shared_ptr<GeomAPI_Edge> lineCircle(std::shared_ptr<GeomAPI_Pnt> theCenter,
                                                    std::shared_ptr<GeomAPI_Dir> theNormal,
                                                    double theRadius);

  /// Creates linear edge in a form of a circle arc by a three points
  static std::shared_ptr<GeomAPI_Edge> lineCircleArc(std::shared_ptr<GeomAPI_Pnt> theCenter,
                                                       std::shared_ptr<GeomAPI_Pnt> theStartPoint,
                                                       std::shared_ptr<GeomAPI_Pnt> theEndPoint,
                                                       std::shared_ptr<GeomAPI_Dir> theNormal);
};

#endif
