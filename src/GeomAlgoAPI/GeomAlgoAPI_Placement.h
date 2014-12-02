// File:        GeomAlgoAPI_Placement.h
// Created:     2 Dec 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAlgoAPI_Placement_H_
#define GeomAlgoAPI_Placement_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Pln.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <memory>

/**\class GeomAlgoAPI_Placement
 * \ingroup DataAlgo
 * \brief Creates the copied object which face is placed on the given plane
 */
class GeomAlgoAPI_Placement : public GeomAPI_Interface
{
public:
  /** \brief Creates an object which is obtained from current object by transformation calculated
   *         as a movement of the source plane to be coincident with the destination plane
   *  \param[in] theAttractiveShape shape to be moved
   *  \param[in] theSourcePlane     plane on the shape to be made coincident with destination plane
   *  \param[in] theDestPlane       destination plane
   */
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Placement(std::shared_ptr<GeomAPI_Shape> theAttractiveShape,
                                           std::shared_ptr<GeomAPI_Pln> theSourcePlane,
                                           std::shared_ptr<GeomAPI_Pln> theDestPlane);

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

  /// Destructor
  GEOMALGOAPI_EXPORT virtual ~GeomAlgoAPI_Placement();

private:
  /// builds resulting shape
  void build(const std::shared_ptr<GeomAPI_Shape>& theAttractiveShape,
             const std::shared_ptr<GeomAPI_Pln>& theSourcePlane,
             const std::shared_ptr<GeomAPI_Pln>& theDestPlane);

  /// fields
  bool myDone;
  std::shared_ptr<GeomAPI_Shape> myShape;
  GeomAPI_DataMapOfShapeShape myMap;
  GeomAlgoAPI_MakeShape * myMkShape;
};

#endif
