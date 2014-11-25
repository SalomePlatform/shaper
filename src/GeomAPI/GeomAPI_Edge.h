// File:        GeomAPI_Edge.hxx
// Created:     24 Jul 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Edge_H_
#define GeomAPI_Edge_H_

#include <GeomAPI_Shape.h>

class GeomAPI_Pnt;
class GeomAPI_Circ;

/**\class GeomAPI_Edge
* \ingroup DataModel
 * \brief Interface to the edge object
 */

class GEOMAPI_EXPORT GeomAPI_Edge : public GeomAPI_Shape
{
public:
   /// Creation of empty (null) shape
   GeomAPI_Edge();

   /// Creation of edge by the edge-shape
   GeomAPI_Edge(const std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Verifies that the edge is a line
  bool isLine() const;

  /// Verifies that the edge is a circle
  bool isCircle() const;

  /// Verifies that the edge is an arc of circle
  bool isArc() const;

  /// Returns the first vertex coordinates of the edge 
  std::shared_ptr<GeomAPI_Pnt> firstPoint();

  /// Returns the Last vertex coordinates of the edge 
  std::shared_ptr<GeomAPI_Pnt> lastPoint();

  /// Returns a circle if edge is based on the cirsle curve
  std::shared_ptr<GeomAPI_Circ> circle();

  /// Returns true if the current edge is geometrically equal to the given edge
  bool isEqual(std::shared_ptr<GeomAPI_Shape> theEdge);
};

#endif

