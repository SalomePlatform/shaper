// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_ShapeExplorer.h
// Created:     5 June 2015
// Author:      Dmitry Bobylev

#ifndef GeomAPI_ShapeExplorer_H_
#define GeomAPI_ShapeExplorer_H_

#include <GeomAPI.h>
#include <GeomAPI_Shape.h>

/** \class GeomAPI_ShapeExplorer
 *  \ingroup DataModel
 *  \brief This class is used to explore subshapes on shape.
 */

class GEOMAPI_EXPORT GeomAPI_ShapeExplorer : public GeomAPI_Interface
{
public:
  /// Default constructor. Creates an empty explorer, becomes usefull after Init.
  GeomAPI_ShapeExplorer();

  /** \brief Constructs an explorer to search on theShape, for shapes of type toFind,
   *  that are not part of a shape toAvoid. If the shape toAvoid is equal to GeomAPI_SHape::SHAPE,
   *  or if it is the same as, or less complex than the shape toFind it has no effect on the search.
      \param[in] toFind shape type to find.
      \param[in] toAvoid shape type to avoid.
   */
  GeomAPI_ShapeExplorer(const std::shared_ptr<GeomAPI_Shape>& theShape,
                        const GeomAPI_Shape::ShapeType toFind,
                        const GeomAPI_Shape::ShapeType toAvoid = GeomAPI_Shape::SHAPE);

  /** \brief Resets this explorer. It is initialized to search on theShape, for shapes of type toFind,
   *  that are not part of a shape toAvoid. If the shape toAvoid is equal to GeomAPI_SHape::SHAPE,
   *  or if it is the same as, or less complex than the shape toFind it has no effect on the search.
      \param[in] toFind shape type to find.
      \param[in] toAvoid shape type to avoid.
   */
  void init(const std::shared_ptr<GeomAPI_Shape>& theShape,
            const GeomAPI_Shape::ShapeType toFind,
            const GeomAPI_Shape::ShapeType toAvoid = GeomAPI_Shape::SHAPE);

  /// \return true if there are more shapes in the exploration.
  bool more() const;

  /// Moves to the next Shape in the exploration or do nothing if there are no more shapes to explore.
  void next();

  /// Returns the current shape in the exploration or empty pointer if this explorer has no more shapes to explore.
  std::shared_ptr<GeomAPI_Shape> current();

  /// Reinitialize the exploration with the original arguments.
  void reinit();

  /// Returns the current depth of the exploration. 0 is the shape to explore itself.
  int depth() const;

  /// Clears the content of the explorer. It will return False on more().
  void clear();

};

#endif