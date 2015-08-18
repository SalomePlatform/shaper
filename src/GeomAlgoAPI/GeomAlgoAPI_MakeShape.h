// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MakeShape.h
// Created:     17 Oct 2014
// Author:      Sergey ZARITCHNY
#ifndef GeomAlgoAPI_MakeShape_H_
#define GeomAlgoAPI_MakeShape_H_

#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI.h>

#include <list>
#include <memory>

/**\class GeomAlgoAPI_MakeShape
 * \ingroup DataAlgo
 * \brief Interface to the root class of all topological shapes constructions
 */
class GeomAlgoAPI_MakeShape : public GeomAPI_Interface
{
public:
  /// Constructor
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShape();
  /// Constructor by the already stored builder in the interface
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShape(void* theBuilder);

  /// Constructor by the builder and wire. Used for pipe builder.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShape(void* theBuilder,
                                           const std::shared_ptr<GeomAPI_Shape> theWire,
                                           const std::shared_ptr<GeomAPI_Shape> theBaseShape);

  /// Returns a shape built by the shape construction algorithm
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>  shape() const;

  /// Returns the list of shapes generated from the shape \a theShape
  GEOMALGOAPI_EXPORT virtual void generated(
    const std::shared_ptr<GeomAPI_Shape> theShape, ListOfShape& theHistory);

  /// Returns the  list of shapes modified from the shape \a theShape
  GEOMALGOAPI_EXPORT virtual void modified(
    const std::shared_ptr<GeomAPI_Shape> theShape, ListOfShape& theHistory);

  /// Returns whether the shape is an edge
  GEOMALGOAPI_EXPORT virtual bool isDeleted(const std::shared_ptr<GeomAPI_Shape> theShape);

  protected:
  /// The resulting shape
  std::shared_ptr<GeomAPI_Shape> myShape;
  std::shared_ptr<GeomAPI_Shape> myWire;
  std::shared_ptr<GeomAPI_Shape> myBaseShape;
};

typedef std::list<std::shared_ptr<GeomAlgoAPI_MakeShape> > ListOfMakeShape;

#endif
