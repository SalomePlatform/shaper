// File:        GeomAPI_DataMapOfShapeShape.h
// Created:     28 Oct 2014
// Author:      Sergey Zaritchny

#ifndef GeomAPI_DataMapOfShapeShape_H_
#define GeomAPI_DataMapOfShapeShape_H_

#include <boost/shared_ptr.hpp>
#include <GeomAPI_Interface.h>

class GeomAPI_Pnt;
class GeomAPI_Dir;

/**\class GeomAPI_DataMapOfShapeShape
 * \ingroup DataModel
 * \brief DataMap of Shape - Shape defined by TopoDS_Shapes
 */

class GEOMAPI_EXPORT GeomAPI_DataMapOfShapeShape : public GeomAPI_Interface
{
 public:
  /// Creation of plane by the point and normal
  GeomAPI_DataMapOfShapeShape();

  /// Clear 
  void clear();

  /// Adds the Key <K> to  the Map <me>  with  the  Item. Returns True  if the Key  was not already in the map
  bool bind (boost::shared_ptr<GeomAPI_Shape> theKey, boost::shared_ptr<GeomAPI_Shape> theItem);

  /// Returns true if theKey is stored  in the map.
  bool isBound (boost::shared_ptr<GeomAPI_Shape> theKey);

  /// Returns  the Item stored  with the Key in the Map.
  const boost::shared_ptr<GeomAPI_Shape> find(boost::shared_ptr<GeomAPI_Shape> theKey);  
  
  /// Removes the Key from the  map. Returns true if the Key was in the Map
  bool unBind(boost::shared_ptr<GeomAPI_Shape> theKey);

  /// Destructor
  ~GeomAPI_DataMapOfShapeShape();
};

#endif

