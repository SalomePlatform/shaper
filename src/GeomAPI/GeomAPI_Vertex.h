// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Vertex.hxx
// Created:     16 Dec 2014
// Author:      Vitaly Smetannikov

#ifndef GeomAPI_Vertex_H_
#define GeomAPI_Vertex_H_

#include <GeomAPI_Shape.h>

class GeomAPI_Pnt;

/**\class GeomAPI_Vertex
* \ingroup DataModel
 * \brief Interface to the vertex object
 */

class GEOMAPI_EXPORT GeomAPI_Vertex : public GeomAPI_Shape
{
public:
   /// Creation of empty (null) shape
   GeomAPI_Vertex();

   /// Creation of edge by the edge-shape
   GeomAPI_Vertex(const std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Returns the first vertex coordinates of the edge 
  std::shared_ptr<GeomAPI_Pnt> point();

  /// Returns true if the current edge is geometrically equal to the given edge
  bool isEqual(const std::shared_ptr<GeomAPI_Shape> theVert) const;
};

#endif

