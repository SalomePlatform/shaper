// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Partition.h
// Created:     21 Aug 2015
// Author:      Sergey POKHODENKO

#ifndef GeomAlgoAPI_Partition_H_
#define GeomAlgoAPI_Partition_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_Interface.h>
#include <GeomAPI_DataMapOfShapeShape.h>

#include <memory>

/** \class GeomAlgoAPI_Partition
 *  \ingroup DataAlgo
 *  \brief Allows to perform of partition operations
 */
class GeomAlgoAPI_Partition : public GeomAPI_Interface
{
public:
  /** \brief Creates common partition operation.
   *  \param[in] theObjects the main shape.
   *  \param[in] theTools  second shape.
   *  \return a solid as result of operation.
   */
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape> make(const ListOfShape& theObjects,
                                                                const ListOfShape& theTools);

  /// Constructor.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Partition(const ListOfShape& theObjects,
                                           const ListOfShape& theTools);

  /// \return true if algorithm succeed.
  GEOMALGOAPI_EXPORT const bool isDone() const;

  /// \return true if resulting shape is valid.
  GEOMALGOAPI_EXPORT const bool isValid() const;

  /// \return result of the boolean algorithm.
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape>& shape() const;

  /// \return map of sub-shapes of the result. To be used for History keeping.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_DataMapOfShapeShape> mapOfShapes() const;

  /// \return interface for for History processing.
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAlgoAPI_MakeShape> makeShape() const;

private:
  /// Builds resulting shape.
  void build(const ListOfShape& theObjects,
             const ListOfShape& theTools);

private:
  /// Fields.
  bool myDone;
  std::shared_ptr<GeomAPI_Shape> myShape;
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> myMap;
  std::shared_ptr<GeomAlgoAPI_MakeShape> myMkShape;
};

#endif
