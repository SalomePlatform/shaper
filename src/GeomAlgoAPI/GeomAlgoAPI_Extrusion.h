// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Extrusion.h
// Created:     22 October 2014
// Author:      Sergey Zaritchny

#ifndef GeomAlgoAPI_Extrusion_H_
#define GeomAlgoAPI_Extrusion_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Dir.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <memory>

/**\class GeomAlgoAPI_Extrusion
 * \ingroup DataAlgo
 * \brief Allows to create the prism based on a given face and a direction
 */

class GeomAlgoAPI_Extrusion : public GeomAPI_Interface
{
 public:

  /* \brief Creates extrusion for the given shape along the normal for this shape
   * \param[in] theBasis face or wire to be extruded
   * \param[in] theSize  the length of extrusion (if the value is less than 0, the extrusion in opposite normal)
   * \return a solid or a face/shell which is obtained from specified one  
   */
  /// Constructor
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Extrusion (std::shared_ptr<GeomAPI_Shape> theBasis, double theSize);

  /// Returns True if algorithm succeed
  GEOMALGOAPI_EXPORT const bool isDone() const;

  ///  Returns True if resulting shape is valid
  GEOMALGOAPI_EXPORT const bool isValid() const;

  /// Returns True if resulting shape has volume
  GEOMALGOAPI_EXPORT const bool hasVolume() const;

  /// Returns result of the Extrusion algorithm which may be a Solid or a Face
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& shape () const;

  /// Returns the first shape 
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& firstShape();

  /// returns last shape
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& lastShape();	
 
  /// Returns map of sub-shapes of the result. To be used for History keeping
  GEOMALGOAPI_EXPORT void  mapOfShapes (GeomAPI_DataMapOfShapeShape& theMap) const;

  /// Return interface for for History processing
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShape* makeShape () const;

  /// Destructor
  GEOMALGOAPI_EXPORT  ~GeomAlgoAPI_Extrusion();
private:
  /// builds resulting shape
  void build(const std::shared_ptr<GeomAPI_Shape>& theBasis);
  /// fields
  double mySize;
  bool myDone;
  std::shared_ptr<GeomAPI_Shape> myShape;
  std::shared_ptr<GeomAPI_Shape> myFirst;
  std::shared_ptr<GeomAPI_Shape> myLast;
  GeomAPI_DataMapOfShapeShape myMap;
  GeomAlgoAPI_MakeShape * myMkShape;
};

#endif
