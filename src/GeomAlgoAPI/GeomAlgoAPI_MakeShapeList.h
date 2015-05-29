// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MakeShapeList.h
// Created:     27 May 2015
// Author:      Dmitry Bobylev
#ifndef GeomAlgoAPI_MakeShapeList_H_
#define GeomAlgoAPI_MakeShapeList_H_

#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <memory>

/** \class GeomAlgoAPI_MakeShapeList
 *  \ingroup DataAlgo
 *  \brief Interface to the root class of all topological shapes constructions
 */
class GeomAlgoAPI_MakeShapeList : public GeomAlgoAPI_MakeShape
{
  enum OperationType {
    Generated,
    Modified
  };

public:
  /// Default constructor
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShapeList();

  /** \brief Constructor
   *  \param[in] theMakeShapeList list of algorithms.
   */
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShapeList(const ListOfMakeShape& theMakeShapeList);

  /** \brief Initializes a class with new list of algorithms.
   *  \param[in] theMakeShapeList list of algorithms.
   */
  GEOMALGOAPI_EXPORT void init(const ListOfMakeShape& theMakeShapeList);

  /// \return a shape built by the shape construction algorithms
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape> shape() const;

  /// \return the list of shapes generated from the shape \a theShape
  GEOMALGOAPI_EXPORT virtual void generated(const std::shared_ptr<GeomAPI_Shape> theShape,
                                            ListOfShape& theHistory);

  /// \return the list of shapes modified from the shape \a theShape
  GEOMALGOAPI_EXPORT virtual void modified(const std::shared_ptr<GeomAPI_Shape> theShape,
                                           ListOfShape& theHistory);

  /// Returns whether the shape is deleted
  GEOMALGOAPI_EXPORT virtual bool isDeleted(const std::shared_ptr<GeomAPI_Shape> theShape);

private:
  void result(const std::shared_ptr<GeomAPI_Shape> theShape,
              ListOfShape& theHistory,
              OperationType theOperationType);


protected:
  ListOfMakeShape myListOfMakeShape;
};

#endif
