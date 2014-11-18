// File:        GeomAPI_Shape.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Shape_H_
#define GeomAPI_Shape_H_

#include <GeomAPI_Interface.h>
#include <memory>
#include <list>

/**\class GeomAPI_Shape
 * \ingroup DataModel
 * \brief Interface to the topological shape object
 */
class GEOMAPI_EXPORT GeomAPI_Shape : public GeomAPI_Interface
{
 public:
  /// Creation of empty (null) shape
  GeomAPI_Shape();

  bool isNull() const;

  /// Returns whether the shapes are equal
  virtual bool isEqual(const std::shared_ptr<GeomAPI_Shape> theShape) const;

  /// Returns whether the shape is a vertex
  virtual bool isVertex() const;

  /// Returns whether the shape is an edge
  virtual bool isEdge() const;

};

//! Pointer on list of shapes
typedef std::list<std::shared_ptr<GeomAPI_Shape> > ListOfShape;

//! Pointer on attribute object
typedef std::shared_ptr<GeomAPI_Shape> GeomShapePtr;

#endif
