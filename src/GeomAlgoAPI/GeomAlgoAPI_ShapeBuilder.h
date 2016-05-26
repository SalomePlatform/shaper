// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ShapeBuilder.h
// Created:     27 April 2016
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_ShapeBuilder_H_
#define GeomAlgoAPI_ShapeBuilder_H_

#include "GeomAlgoAPI.h"
#include "GeomAlgoAPI_MakeShapeList.h"

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_ShapeBuilder
/// \ingroup DataAlgo
/// \brief Allows to add or remove subshapes from a shape.
class GeomAlgoAPI_ShapeBuilder: public GeomAlgoAPI_MakeShapeList
{
 public:
   /// \brief Adds theShapeToAdd in theShape.
   /// \param[in] theShape shape to modify.
   /// \param[in] theShapeToAdd shape which will be added.
   GEOMALGOAPI_EXPORT static void add(std::shared_ptr<GeomAPI_Shape> theShape,
                                      const std::shared_ptr<GeomAPI_Shape> theShapeToAdd);

   /// \brief Removes theShapeToRemove in theShape.
   /// \param[in] theShape shape to modify.
   /// \param[in] theShapeToRemove shape which will be removed.
   GEOMALGOAPI_EXPORT static void remove(std::shared_ptr<GeomAPI_Shape> theShape,
                                         const std::shared_ptr<GeomAPI_Shape> theShapeToRemove);

   /// Creates empty builder.
   GEOMALGOAPI_EXPORT GeomAlgoAPI_ShapeBuilder();

   /// \brief Store new shape as result of removing internal shapes from theShape.
   /// \param[in] theShape base shape.
   GEOMALGOAPI_EXPORT void removeInternal(const std::shared_ptr<GeomAPI_Shape> theShape);

   /// \brief Store new shape as result of adding theShapesToAdd to theShape as internal shapes.
   /// \param[in] theShape base shape.
   /// \param[in] theShapesToAdd shapes which will be added.
   GEOMALGOAPI_EXPORT void addInternal(const std::shared_ptr<GeomAPI_Shape> theShape,
                                       const ListOfShape& theShapesToAdd);
};

#endif
