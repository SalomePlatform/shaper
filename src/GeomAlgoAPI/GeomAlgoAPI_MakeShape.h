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
class GeomAlgoAPI_MakeShape : GeomAPI_Interface
{
 public:
   /// Constructor
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShape(void* theBuilder);
  /// Returns a shape built by the shape construction algorithm
  GEOMALGOAPI_EXPORT const boost::shared_ptr<GeomAPI_Shape>  shape() const;

  /// Returns the  list   of shapes generated   from the shape <theShape>
  GEOMALGOAPI_EXPORT virtual void generated(
    const boost::shared_ptr<GeomAPI_Shape> theShape, ListOfShape& theHistory);

  /// Returns the  list   of shapes modified   from the shape <theShape>
  GEOMALGOAPI_EXPORT virtual void modified(
    const boost::shared_ptr<GeomAPI_Shape> theShape, ListOfShape& theHistory);

  /// Returns whether the shape is an edge
  GEOMALGOAPI_EXPORT virtual bool isDeleted(const boost::shared_ptr<GeomAPI_Shape> theShape);

  protected:
	boost::shared_ptr<GeomAPI_Shape> myShape;
};

#endif
