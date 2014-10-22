// File:        GeomAPI_Wire.hxx
// Created:     24 Jul 2014
// Author:      Artem ZHIDKOV

#ifndef GEOMAPI_WIRE_H_
#define GEOMAPI_WIRE_H_

#include "GeomAPI.h"
#include "GeomAPI_Edge.h"
#include "GeomAPI_Pnt.h"
#include "GeomAPI_Dir.h"

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

  /// Returns True if the wire is defined in a plane
  bool hasPlane() const { return myOrigin && myNorm && myDirX && myDirY; }

  /// Set/Get origin point
  void setOrigin(const boost::shared_ptr<GeomAPI_Pnt>& theOrigin) { myOrigin = theOrigin; }
  boost::shared_ptr<GeomAPI_Pnt> origin() const { return myOrigin; }

  /// Set/Get X direction vector
  void setDirX(const boost::shared_ptr<GeomAPI_Dir>& theDirX) { myDirX = theDirX; }
  boost::shared_ptr<GeomAPI_Dir> dirX() const { return myDirX; }

  /// Set/Get Y direction vector
  void setDirY(const boost::shared_ptr<GeomAPI_Dir>& theDirY) { myDirY = theDirY; }
  boost::shared_ptr<GeomAPI_Dir> dirY() const { return myDirY; }

  /// Set/Get Normal direction vector
  void setNorm(const boost::shared_ptr<GeomAPI_Dir>& theNorm) { myNorm = theNorm; }
  boost::shared_ptr<GeomAPI_Dir> norm() const { return myNorm; }

private:
  boost::shared_ptr<GeomAPI_Pnt> myOrigin;
  boost::shared_ptr<GeomAPI_Dir> myDirX;
  boost::shared_ptr<GeomAPI_Dir> myDirY;
  boost::shared_ptr<GeomAPI_Dir> myNorm;
};

#endif

