// File:        GeomAlgoAPI_MakeShape.h
// Created:     17 Oct 2014
// Author:      Sergey ZARITCHNY
#ifndef GeomAlgoAPI_MakeShape_H_
#define GeomAlgoAPI_MakeShape_H_

#include <GeomAPI_Shape.h>
#include <boost/shared_ptr.hpp>
#include <GeomAlgoAPI.h>
#include <BRepBuilderAPI_MakeShape.hxx>
/**\class GeomAlgoAPI_MakeShape
 * \ingroup DataModel
 * \Interface to the root class of all topological shapes constructions
 */
class GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShape
{
 public:
   /// Constructor
  GeomAlgoAPI_MakeShape(BRepBuilderAPI_MakeShape * theBuilder);
  /// Returns a shape built by the shape construction algorithm
  const boost::shared_ptr<GeomAPI_Shape>  shape() const;

  /// Returns the  list   of shapes generated   from the shape <theShape>
  virtual const ListOfShape& generated(const boost::shared_ptr<GeomAPI_Shape> theShape);

  /// Returns the  list   of shapes modified   from the shape <theShape>
  virtual const ListOfShape& modified(const boost::shared_ptr<GeomAPI_Shape> theShape);

  /// Returns whether the shape is an edge
  virtual bool isDeleted(const boost::shared_ptr<GeomAPI_Shape> theShape);

  protected:
	boost::shared_ptr<GeomAPI_Shape> myShape;
	ListOfShape myHistory;
	BRepBuilderAPI_MakeShape * myBuilder;
};

#endif