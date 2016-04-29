// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_ShapeIterator.h
// Created:     27 April 2016
// Author:      Dmitry Bobylev

#ifndef GeomAPI_ShapeIterator_H_
#define GeomAPI_ShapeIterator_H_

#include "GeomAPI.h"
#include "GeomAPI_Shape.h"

/// \class GeomAPI_ShapeIterator
/// \ingroup DataModel
/// \brief Iterates on the underlying shape underlying a given GeomAPI_Shape object, providing access
///        to its component sub-shapes. Each component shape is returned as a GeomAPI_Shape with
///        an orientation, and a compound of the original values and the relative values.
class GeomAPI_ShapeIterator : public GeomAPI_Interface
{
public:
  /// Default constructor. Creates an empty iterator, becomes usefull after Init.
  GEOMAPI_EXPORT GeomAPI_ShapeIterator();

  /// \brief Creates an iterator on theShape sub-shapes.
  /// \param[in] theShape shape to iterate.
  GEOMAPI_EXPORT GeomAPI_ShapeIterator(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \brief Initializes this iterator with theShape.
  /// \param[in] theShape shape to iterate.
  GEOMAPI_EXPORT void init(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \return  true if there is another sub-shape in the shape which this iterator is scanning.
  GEOMAPI_EXPORT bool more() const;

  /// Moves on to the next sub-shape in the shape which this iterator is scanning.
  GEOMAPI_EXPORT void next();

  /// \return the current sub-shape in the shape which this iterator is scanning.
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Shape> current();
};

#endif