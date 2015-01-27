// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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

  /// Returns true if the underlied shape is null
  bool isNull() const;

  /// Returns whether the shapes are equal
  virtual bool isEqual(const std::shared_ptr<GeomAPI_Shape> theShape) const;

  /// Returns whether the shape is a vertex
  virtual bool isVertex() const;

  /// Returns whether the shape is an edge
  virtual bool isEdge() const;

  /// Returns whether the shape is a face
  virtual bool isFace() const;

  /// Computes boundary dimensions of the shape
  /// Returns False if it is not possible
  bool computeSize(double& theXmin, double& theYmin, double& theZmin,
                   double& theXmax, double& theYmax, double& theZmax) const;

  /// Returns the shape as BRep stream
  std::string getShapeStream() const;
};

//! Pointer on list of shapes
typedef std::list<std::shared_ptr<GeomAPI_Shape> > ListOfShape;

//! Pointer on attribute object
typedef std::shared_ptr<GeomAPI_Shape> GeomShapePtr;

#endif
