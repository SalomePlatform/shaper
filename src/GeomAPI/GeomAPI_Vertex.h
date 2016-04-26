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

class GeomAPI_Vertex : public GeomAPI_Shape
{
public:
   /// Creation of empty (null) shape
  GEOMAPI_EXPORT 
   GeomAPI_Vertex();

   /// Creation of vertex by the vertex-shape
  GEOMAPI_EXPORT 
   GeomAPI_Vertex(const std::shared_ptr<GeomAPI_Shape>& theShape);

   /// Creation of vertex by 3d coordinates
  GEOMAPI_EXPORT 
   GeomAPI_Vertex(double theX, double theY, double theZ);

  /// Returns the first vertex coordinates of the edge 
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Pnt> point();

  /// Returns true if the current edge is geometrically equal to the given edge
  GEOMAPI_EXPORT 
  bool isEqual(const std::shared_ptr<GeomAPI_Shape> theVert) const;
};

#endif

