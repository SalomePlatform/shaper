// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef GeomAPI_ShapeHierarchy_H_
#define GeomAPI_ShapeHierarchy_H_

#include "GeomAPI.h"
#include "GeomAPI_Shape.h"

#include <map>
#include <set>
#include <vector>

/// \class GeomAPI_ShapeHierarchy
/// \ingroup Plugins
/// \brief Storage for the hierarchy of shapes and their parents (compounds or compsolids)
class GeomAPI_ShapeHierarchy
{
  typedef std::pair<GeomShapePtr, ListOfShape> ShapeAndSubshapes;
  typedef std::map<GeomShapePtr, GeomShapePtr, GeomAPI_Shape::Comparator> MapShapeToShape;
  typedef std::map<GeomShapePtr, size_t, GeomAPI_Shape::Comparator> MapShapeToIndex;
  typedef std::set<GeomShapePtr, GeomAPI_Shape::Comparator> SetOfShape;

  ListOfShape myObjects; ///< list of objects of some operation
  MapShapeToShape myParent; ///< refer a shape to compound/compsolid containing it
  /// indices of compounds/compsolids to keep the order of parent shapes
  /// corresponding to the order of objects
  MapShapeToIndex  myParentIndices;
  /// list of shape and its subshapes stored according to the index of parent shape
  std::vector<ShapeAndSubshapes> mySubshapes;

  SetOfShape myProcessedObjects;
  MapShapeToShape myModifiedObjects;

public:
  /// Add an object of the operation (low-level shape in the hierarchy)
  GEOMAPI_EXPORT void addObject(const GeomShapePtr& theObject);

  /// Store link between shape and its parent.
  /// Has to be called by high-level algorithm, because the parent compound/compsolid
  /// is usually stored as a top-level result
  GEOMAPI_EXPORT void addParent(const GeomShapePtr& theShape, const GeomShapePtr& theParent);

  /// Return parent shape for the given, or empty if it is a high-level shape.
  /// By default, the parent and all its subshapes are marked as processed for further skip.
  GEOMAPI_EXPORT GeomShapePtr parent(const GeomShapePtr& theShape, bool theMarkProcessed = true);

  /// Mark the shape as already processed
  GEOMAPI_EXPORT void markProcessed(const GeomShapePtr& theShape);
  /// Mark list ofshapes as already processed
  GEOMAPI_EXPORT void markProcessed(const ListOfShape& theShapes);

  /// Mark the shape as modified and store its image
  GEOMAPI_EXPORT void markModified(const GeomShapePtr& theSource, const GeomShapePtr& theModified);

  /// Split compound/compsolid shape for subshapes selected for operation and the others.
  GEOMAPI_EXPORT void splitCompound(const GeomShapePtr& theCompShape,
                                    ListOfShape& theUsed,
                                    ListOfShape& theNotUsed) const;

  /// Generates the list of top-level compounds, which exclude the objects of operation.
  GEOMAPI_EXPORT void compoundsOfUnusedObjects(ListOfShape& theDestination) const;

  /// Generates the list of top-level compounds, with modified objects of operation.
  GEOMAPI_EXPORT void topLevelObjects(ListOfShape& theDestination) const;

  /// Return \c true if there is no object in hierarchy
  GEOMAPI_EXPORT bool empty() const;

  /// Return list of objects
  const ListOfShape& objects() const { return myObjects; }
  /// Separate objects of the given range of types and all other objects
  GEOMAPI_EXPORT void objectsByType(ListOfShape& theShapesByType, ListOfShape& theOtherShapes,
      const GeomAPI_Shape::ShapeType theMinType = GeomAPI_Shape::COMPOUND,
      const GeomAPI_Shape::ShapeType theMaxType = GeomAPI_Shape::SHAPE) const;

private:
  /// Collect subs of a top-level compound, excluding the set of given objects
  /// and substitute the shapes which were modified
  GeomShapePtr collectSubs(const GeomShapePtr theTopLevelCompound,
                           const SetOfShape& theExcluded = SetOfShape(),
                           const MapShapeToShape& theModified = MapShapeToShape()) const;

public:
  class iterator : public std::iterator<std::forward_iterator_tag, GeomShapePtr>
  {
  public:
    GEOMAPI_EXPORT iterator() {}

  protected:
    iterator(GeomAPI_ShapeHierarchy* theHierarchy, bool isBegin = true);

    void skipAlreadyProcessed();

  public:
    GEOMAPI_EXPORT bool operator==(const iterator&) const;
    GEOMAPI_EXPORT bool operator!=(const iterator&) const;

    GEOMAPI_EXPORT iterator& operator++();
    GEOMAPI_EXPORT iterator  operator++(int);

    GEOMAPI_EXPORT GeomShapePtr operator*() const;

  private:
    GeomAPI_ShapeHierarchy* myHierarchy;
    ListOfShape::iterator myObject;

    friend class GeomAPI_ShapeHierarchy;
  };

  GEOMAPI_EXPORT iterator begin();
  GEOMAPI_EXPORT iterator end();
};

#endif
