// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Placement.h
// Created:     2 Dec 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAlgoAPI_Placement_H_
#define GeomAlgoAPI_Placement_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Trsf.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <memory>

/**\class GeomAlgoAPI_Placement
 * \ingroup DataAlgo
 * \brief Creates the copied object which sub-element is placed on the given element
 */
class GeomAlgoAPI_Placement : public GeomAPI_Interface
{
public:
  /** \brief Creates an object which is obtained from current object by transformation calculated
   *         as a movement of the source object to be coincident with the destination object
   *  \param[in] theSourceSolid  a shape to be moved
   *  \param[in] theDestSolid    invariant shape
   *  \param[in] theSourceShape  a shape on the solid to be made coincident with destination object
   *  \param[in] theDestShape    destination object
   *  \param[in] theIsReverse    indicates that the solid materials should be on the same side against the destination plane
   *  \param[in] theIsCentering  indicates the planes should be centered
   *  \param[in] theSimpleTransform makes just transformation of shape without changing of topology or geometry
   */
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Placement(std::shared_ptr<GeomAPI_Shape> theSourceSolid,
                                           std::shared_ptr<GeomAPI_Shape> theDestSolid,
                                           std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Shape> theDestShape,
                                           bool theIsReverse = false,
                                           bool theIsCentering = false,
                                           bool theSimpleTransform = false);

  /// Returns True if algorithm succeed
  GEOMALGOAPI_EXPORT const bool isDone() const
  { return myDone; }

  ///  Returns True if resulting shape is valid
  GEOMALGOAPI_EXPORT const bool isValid() const;

  /// Returns True if resulting shape has volume
  GEOMALGOAPI_EXPORT const bool hasVolume() const;

  /// Returns result of the Placement algorithm which may be a Solid or a Face
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& shape () const;

  /// Returns map of sub-shapes of the result. To be used for History keeping
  GEOMALGOAPI_EXPORT void  mapOfShapes (GeomAPI_DataMapOfShapeShape& theMap) const;

  /// Return interface for for History processing
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShape* makeShape () const;

  /// Returns the simple transformation
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_Trsf> transformation() const;

  /// Destructor
  GEOMALGOAPI_EXPORT virtual ~GeomAlgoAPI_Placement();

private:
  /// builds resulting shape
  void build(const std::shared_ptr<GeomAPI_Shape>& theSourceSolid,
             const std::shared_ptr<GeomAPI_Shape>& theDestSolid,
             const std::shared_ptr<GeomAPI_Shape>& theSourceShape,
             const std::shared_ptr<GeomAPI_Shape>& theDestShape,
             bool theIsReverse,
             bool theIsCentering,
             bool theSimpleTransform);

  /// fields
  bool myDone;
  std::shared_ptr<GeomAPI_Shape> myShape;
  GeomAPI_DataMapOfShapeShape myMap;
  GeomAlgoAPI_MakeShape * myMkShape;
  std::shared_ptr<GeomAPI_Trsf> myTrsf; ///< transformation of the shape in case theSimpleTransform
};

#endif
