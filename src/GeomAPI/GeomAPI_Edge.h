// File:        GeomAPI_Edge.hxx
// Created:     24 Jul 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Edge_H_
#define GeomAPI_Edge_H_

#include <GeomAPI_Shape.h>

/**\class GeomAPI_Edge
 * \ingroup DataModel
 * \brief Interface to the edge object
 */

class GEOMAPI_EXPORT GeomAPI_Edge: public GeomAPI_Shape
{
public:
  /// Creation of empty (null) shape
  GeomAPI_Edge();

  /// Returns whether the shape is a vertex
  virtual bool isVertex() const
  { return false; }

  /// Returns whether the shape is an edge
  virtual bool isEdge() const
  { return true; }

  /// Verifies that the edge is a line
  bool isLine() const;

  /// Verifies that the edge is a circle
  bool isCircle() const;

  /// Verifies that the edge is an arc of circle
  bool isArc() const;
};

#endif

