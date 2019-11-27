// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef FeaturesPlugin_VersionedBoolean_H_
#define FeaturesPlugin_VersionedBoolean_H_

#include "FeaturesPlugin.h"
#include "FeaturesPlugin_Tools.h"

#include <GeomAlgoAPI_Tools.h>

#include <ModelAPI_Feature.h>

class ModelAPI_Attribute;
class ModelAPI_Result;
class GeomAlgoAPI_MakeShapeList;

/// \class FeaturesPlugin_VersionedBoolean
/// \ingroup Plugins
/// \brief Feature controls a version of Boolean operations.
class FeaturesPlugin_VersionedBoolean : public ModelAPI_Feature
{
public:
  static const int THE_VERSION_0 = 0;
  static const int THE_VERSION_1 = 20190506;

  /// Attribute name of the version of Boolean feature
  inline static const std::string& VERSION_ID()
  {
    static const std::string MY_VERSION_ID("version");
    return MY_VERSION_ID;
  }

protected:

  /// Use plugin manager for features creation.
  FeaturesPlugin_VersionedBoolean() {}

  /// Initialize version field of the Boolean feature.
  /// The version is initialized for newly created features,
  /// not read from previously stored document.
  void initVersion(const int theVersion,
                   const std::shared_ptr<ModelAPI_Attribute> theObjectsAttr
                            = std::shared_ptr<ModelAPI_Attribute>(),
                   const std::shared_ptr<ModelAPI_Attribute> theToolsAttr
                            = std::shared_ptr<ModelAPI_Attribute>());

  /// Auxiliary class to store hierarchy of Boolean operation objects/tools
  /// and their parent shapes (compounds or compsolids)
  class ObjectHierarchy {
    typedef std::pair<GeomShapePtr, ListOfShape> ShapeAndSubshapes;
    typedef std::map<GeomShapePtr, GeomShapePtr, GeomAPI_Shape::Comparator> MapShapeToParent;
    typedef std::map<GeomShapePtr, size_t, GeomAPI_Shape::Comparator> MapShapeToIndex;
    typedef std::set<GeomShapePtr, GeomAPI_Shape::Comparator> SetOfShape;

    ListOfShape myObjects; ///< list of objects/tools of Boolean operation
    MapShapeToParent myParent; ///< refer a shape to compound/compsolid containing it
    /// indices of compounds/compsolids to keep the order of parent shapes
    /// corresponding to the order of objects
    MapShapeToIndex  myParentIndices;
    /// list of shape and its subshapes stored according to the index of parent shape
    std::vector<ShapeAndSubshapes> mySubshapes;

    SetOfShape myProcessedObjects;

  public:
    /// Add object of Boolean opration
    void AddObject(const GeomShapePtr& theObject);

    /// Maps shape and its parent
    void AddParent(const GeomShapePtr& theShape, const GeomShapePtr& theParent);

    /// Return parent shape for the given, or empty if it is a high-level shape.
    /// By default, the parent and all its subshapes are marked as processed for further skip.
    GeomShapePtr Parent(const GeomShapePtr& theShape, bool theMarkProcessed = true);

    /// Marke the shape as already processed
    void MarkProcessed(const GeomShapePtr& theShape);
    /// Marke list ofshapes as already processed
    void MarkProcessed(const ListOfShape& theShapes);

    /// Split compound/compsolid shape for subshapes selected for Boolean operation and the other.
    void SplitCompound(const GeomShapePtr& theCompShape,
                       ListOfShape& theUsed,
                       ListOfShape& theNotUsed) const;

    /// Generates the list of top-level compounds, which contain the objects of Boolean operation.
    /// The generated list will contain only shapes unused during the Boolean operation.
    void CompoundsOfUnusedObjects(ListOfShape& theDestination) const;

    /// Return \c true if there is no object in hierarchy
    bool IsEmpty() const;

    /// Return list of objects
    const ListOfShape& Objects() const { return myObjects; }
    /// Separate objects of the given range of types and all other objects
    void ObjectsByType(ListOfShape& theShapesByType, ListOfShape& theOtherShapes,
        const GeomAPI_Shape::ShapeType theMinType = GeomAPI_Shape::COMPOUND,
        const GeomAPI_Shape::ShapeType theMaxType = GeomAPI_Shape::SHAPE) const;

  private:
    GeomShapePtr collectUnusedSubs(const GeomShapePtr theTopLevelCompound,
                                   const SetOfShape& theUsed) const;

  public:
    class Iterator {
      friend class ObjectHierarchy;

      ObjectHierarchy* myHierarchy;
      ListOfShape::iterator myObject;

      Iterator() {}
      Iterator(ObjectHierarchy* theHierarchy, bool isBegin = true);

      void SkipAlreadyProcessed();

    public:
      bool operator==(const Iterator&) const;
      bool operator!=(const Iterator&) const;

      Iterator& operator++();
      Iterator  operator++(int);

      GeomShapePtr operator*() const;
    };

    Iterator Begin();
    Iterator End();
  };

  /// Process SelectionList attribute and fill the objects hierarchy.
  bool processAttribute(const std::string& theAttributeName,
                        ObjectHierarchy& theObjects,
                        ListOfShape& thePlanesList);

  /// Perform Boolean operation of the object with the tools.
  /// In case of theResultCompound is not empty, the result of Boolean operation
  /// is added to this compound, and corresponding ResultBody is not generated.
  /// \return \c false if something went wrong
  bool processObject(const GeomAlgoAPI_Tools::BOPType theBooleanType,
                     const GeomShapePtr& theObject,
                     const ListOfShape& theTools,
                     const ListOfShape& thePlanes,
                     int& theResultIndex,
                     std::vector<FeaturesPlugin_Tools::ResultBaseAlgo>& theResultBaseAlgoList,
                     ListOfShape& theResultShapesList,
                     GeomShapePtr theResulCompound = GeomShapePtr());

  /// Perform Boolean operation of the Compsolid with the tools
  /// In case of theResultCompound is not empty, the result of Boolean operation
  /// is added to this compound, and corresponding ResultBody is not generated.
  /// \return \c false if something went wrong
  bool processCompsolid(const GeomAlgoAPI_Tools::BOPType theBooleanType,
                        ObjectHierarchy& theCompsolidHierarchy,
                        const GeomShapePtr& theCompsolid,
                        const ListOfShape& theTools,
                        const ListOfShape& thePlanes,
                        int& theResultIndex,
                        std::vector<FeaturesPlugin_Tools::ResultBaseAlgo>& theResultBaseAlgoList,
                        ListOfShape& theResultShapesList,
                        GeomShapePtr theResulCompound = GeomShapePtr());

  /// Perform Boolean operation of the Compound with the tools
  /// In case of theResultCompound is not empty, the result of Boolean operation
  /// is added to this compound, and corresponding ResultBody is not generated.
  /// \return \c false if something went wrong
  bool processCompound(const GeomAlgoAPI_Tools::BOPType theBooleanType,
                       ObjectHierarchy& theCompoundHierarchy,
                       const GeomShapePtr& theCompound,
                       const ListOfShape& theTools,
                       int& theResultIndex,
                       std::vector<FeaturesPlugin_Tools::ResultBaseAlgo>& theResultBaseAlgoList,
                       ListOfShape& theResultShapesList,
                       GeomShapePtr theResulCompound = GeomShapePtr());

  /// Resize planes to fit them to the bounding box of the given lins of objects.
  static void resizePlanes(const ListOfShape& theObjects,
                           ListOfShape& thePlanes,
                           std::shared_ptr<GeomAlgoAPI_MakeShapeList>& theMakeShapeList);

  /// Process unused sub-shapes of compounds.
  /// Keep the compound hierarchy, but merge top-level compounds
  /// into a single compound and add the result of the FUSE operation.
  GeomShapePtr keepUnusedSubsOfCompound(
      const GeomShapePtr& theResult,
      const ObjectHierarchy& theObjectsHierarchy,
      const ObjectHierarchy& theToolsHierarchy,
      std::shared_ptr<GeomAlgoAPI_MakeShapeList> theMakeShapeList);

  /// Return version of the feature
  int version();

private:
  void parentForShape(const GeomShapePtr& theShape,
                      const std::shared_ptr<ModelAPI_Result>& theContext,
                      ObjectHierarchy& theShapesHierarchy);
};

#endif
