// File:        GeomAPI_DataMapOfShapeShape.h
// Created:     28 Oct 2014
// Author:      Sergey Zaritchny

#ifndef GeomAPI_DataMapOfShapeShape_H_
#define GeomAPI_DataMapOfShapeShape_H_

#include <memory>
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

  /// Size of the map
  int size();

  /// Adds the Key <K> to  the Map <me>  with  the  Item. Returns True  if the Key  was not already in the map
  bool bind (std::shared_ptr<GeomAPI_Shape> theKey, std::shared_ptr<GeomAPI_Shape> theItem);

  /// Returns true if theKey is stored  in the map.
  bool isBound (std::shared_ptr<GeomAPI_Shape> theKey);

  /// Returns  the Item stored  with the Key in the Map.
  const std::shared_ptr<GeomAPI_Shape> find(std::shared_ptr<GeomAPI_Shape> theKey);  
  
  /// Removes the Key from the  map. Returns true if the Key was in the Map
  bool unBind(std::shared_ptr<GeomAPI_Shape> theKey);

  /// Destructor
  ~GeomAPI_DataMapOfShapeShape();
};

#endif

