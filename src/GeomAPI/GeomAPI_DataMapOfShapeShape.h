// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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
class GeomAPI_DataMapOfShapeShape : public GeomAPI_Interface
{
 public:
  /// Creation of plane by the point and normal
  GEOMAPI_EXPORT 
  GeomAPI_DataMapOfShapeShape();

  /// Clear 
  GEOMAPI_EXPORT 
  void clear();

  /// Size of the map
  GEOMAPI_EXPORT 
  int size();

  /// Adds \a theKey to me with \a theItem. Returns True if the Key  was not already in the map
  GEOMAPI_EXPORT 
  bool bind (std::shared_ptr<GeomAPI_Shape> theKey, std::shared_ptr<GeomAPI_Shape> theItem);

  /// Merges two maps
  GEOMAPI_EXPORT void merge(const GeomAPI_DataMapOfShapeShape& theDataMap);

  /// Merges two maps
  GEOMAPI_EXPORT void merge(const std::shared_ptr<GeomAPI_DataMapOfShapeShape> theDataMap);

  /// Returns true if theKey is stored  in the map.
  GEOMAPI_EXPORT 
  bool isBound (std::shared_ptr<GeomAPI_Shape> theKey);

  /// Returns  the Item stored  with the Key in the Map.
  GEOMAPI_EXPORT 
  const std::shared_ptr<GeomAPI_Shape> find(std::shared_ptr<GeomAPI_Shape> theKey);  
  
  /// Removes the Key from the  map. Returns true if the Key was in the Map
  GEOMAPI_EXPORT 
  bool unBind(std::shared_ptr<GeomAPI_Shape> theKey);

  /// Destructor
  GEOMAPI_EXPORT 
  ~GeomAPI_DataMapOfShapeShape();
};

#endif

