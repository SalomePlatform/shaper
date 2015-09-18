// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Boolean.h
// Created:     02 Sept 2014
// Author:      Vitaly Smetannikov

#ifndef GeomAlgoAPI_Boolean_H_
#define GeomAlgoAPI_Boolean_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_Interface.h>
#include <GeomAPI_DataMapOfShapeShape.h>

#include <memory>

/** \class GeomAlgoAPI_Boolean
 *  \ingroup DataAlgo
 *  \brief Allows to perform of boolean operations
 */
class GeomAlgoAPI_Boolean : public GeomAPI_Interface
{
public:
  /// Type of booelan operation
  enum OperationType{
    BOOL_CUT, ///< Cut objects
    BOOL_FUSE, ///< Fuse objects
    BOOL_COMMON ///< Take common part of objects
  };

 public:
  /** \brief Creates cut boolean operation.
   *  \param[in] theObjects the main shape.
   *  \param[in] theTools  toole shape for boolean.
   *  \return a solid or compound of solids as result of operation.
   */
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape> makeCut(const ListOfShape& theObjects,
                                                                   const ListOfShape& theTools);

  /** \brief Creates fuse boolean operation.
   *  \param[in] theObjects the main shape.
   *  \param[in] theTools  second shape.
   *  \return a solid as result of operation.
   */
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape> makeFuse(const ListOfShape& theObjects,
                                                                    const ListOfShape& theTools);

  /** \brief Creates common boolean operation.
   *  \param[in] theObjects the main shape.
   *  \param[in] theTools  second shape.
   *  \return a solid as result of operation.
   */
  GEOMALGOAPI_EXPORT static std::shared_ptr<GeomAPI_Shape> makeCommon(const ListOfShape& theObjects,
                                                                      const ListOfShape& theTools);

  /// Constructor.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Boolean(const ListOfShape& theObjects,
                                         const ListOfShape& theTools,
                                         const OperationType theOperationType);

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
             const ListOfShape& theTools,
             const OperationType theOperationType);

private:
  /// Fields.
  bool myDone;
  std::shared_ptr<GeomAPI_Shape> myShape;
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> myMap;
  std::shared_ptr<GeomAlgoAPI_MakeShape> myMkShape;
};

#endif
