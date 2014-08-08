// File:        GeomAPI_Shape.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Shape_H_
#define GeomAPI_Shape_H_

#include <GeomAPI_Interface.h>

/**\class GeomAPI_Shape
 * \ingroup DataModel
 * \brief Interface to the topological shape object
 */
class TopoDS_Shape;

#define MY_SHAPE static_cast<TopoDS_Shape*>(myImpl)

class GEOMAPI_EXPORT GeomAPI_Shape: public GeomAPI_Interface
{
public:
  /// Creation of empty (null) shape
  GeomAPI_Shape();

  bool isNull();

  /// Returns whether the shape is a vertex
  virtual bool isVertex() const;

  /// Returns whether the shape is an edge
  virtual bool isEdge() const;

};

#endif

