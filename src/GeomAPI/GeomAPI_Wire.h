// File:        GeomAPI_Wire.hxx
// Created:     24 Jul 2014
// Author:      Artem ZHIDKOV

#ifndef GEOMAPI_WIRE_H_
#define GEOMAPI_WIRE_H_

#include <GeomAPI.h>
#include <GeomAPI_Edge.h>

#include <boost/smart_ptr/shared_ptr.hpp>

#include <list>

/**\class GeomAPI_Wire
 * \ingroup DataModel
 * \brief Interface to the edge object
 */

class GEOMAPI_EXPORT GeomAPI_Wire : public GeomAPI_Shape
{
 public:
  /// Creation of empty (null) shape
  GeomAPI_Wire();

  virtual bool isVertex() const
  {
    return false;
  }

  /// Returns whether the shape is an edge
  virtual bool isEdge() const
  {
    return false;
  }

  void addEdge(boost::shared_ptr<GeomAPI_Shape> theEdge);
  std::list<boost::shared_ptr<GeomAPI_Shape> > getEdges();

};

#endif

