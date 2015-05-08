// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Prism.h
// Created:     5 May 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_Prism_H_
#define GeomAlgoAPI_Prism_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <memory>

/**\class GeomAlgoAPI_Prism
 * \ingroup DataAlgo
 * \brief Allows to create the prism based on a given face and bounding planes
 */

class GeomAlgoAPI_Prism : public GeomAPI_Interface
{
public:
  /* \brief Creates extrusion for the given shape along the normal for this shape
   * \param[in] theBasis face or wire to be extruded
   * \param[in] theFromShape bottom bounding shape
   * \param[in] theToShape top bounding shape
   * \return a solid or a face/shell which is obtained from specified one
   */
  /// Constructor
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Prism(std::shared_ptr<GeomAPI_Shape> theBasis,
                                       std::shared_ptr<GeomAPI_Shape> theFromShape,
                                       std::shared_ptr<GeomAPI_Shape> theToShape);

  /// Returns True if algorithm succeed
  GEOMALGOAPI_EXPORT const bool isDone() const;

  ///  Returns True if resulting shape is valid
  GEOMALGOAPI_EXPORT const bool isValid() const;

  /// Returns True if resulting shape has volume
  GEOMALGOAPI_EXPORT const bool hasVolume() const;

  /// Returns result of the Prism algorithm which may be a Solid or a Face
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& shape() const;

  /// Returns the first shape 
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& firstShape();

  /// returns last shape
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& lastShape();
 
  /// Returns map of sub-shapes of the result. To be used for History keeping
  GEOMALGOAPI_EXPORT void  mapOfShapes(GeomAPI_DataMapOfShapeShape& theMap) const;

  /// Return interface for for History processing
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShape* makeShape() const;

  /// Destructor
  GEOMALGOAPI_EXPORT  ~GeomAlgoAPI_Prism();
private:
  /// builds resulting shape
  void build(const std::shared_ptr<GeomAPI_Shape>& theBasis);
  /// fields
  std::shared_ptr<GeomAPI_Shape> myFromShape;
  std::shared_ptr<GeomAPI_Shape> myToShape;
  bool myDone;
  std::shared_ptr<GeomAPI_Shape> myShape;
  std::shared_ptr<GeomAPI_Shape> myFirst;
  std::shared_ptr<GeomAPI_Shape> myLast;
  GeomAPI_DataMapOfShapeShape myMap;
  GeomAlgoAPI_MakeShape* myMkShape;
};

#endif
