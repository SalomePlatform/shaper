// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef Model_BodyBuilder_H_
#define Model_BodyBuilder_H_

#include "Model.h"
#include <ModelAPI_BodyBuilder.h>

#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <vector>

class TNaming_Builder;

/**\class Model_BodyBuilder
 * \ingroup DataModel
 * \brief Extra API for the ResultBody that allows to work with naming.
 */
class Model_BodyBuilder : public ModelAPI_BodyBuilder
{
  /// builders that store the naming history: one per label to allow store several shapes to one
  /// label; index in vector corresponds to the label tag
  std::map<int, TNaming_Builder*> myBuilders;
public:
  /// Stores the shape (called by the execution method).
  MODEL_EXPORT virtual void store(const GeomShapePtr& theShape,
                                  const bool theIsStoreSameShapes = true);

  /// Stores the generated shape (called by the execution method).
  MODEL_EXPORT virtual void storeGenerated(const GeomShapePtr& theFromShape,
                                           const GeomShapePtr& theToShape);

  /// Stores the modified shape (called by the execution method).
  /// \param theOldShape shape that produces result
  /// \param theNewShape resulting shape
  /// \param theDecomposeSolidsTag tag for starting of solids sub-elements placement in case
  ///          theNewShape is compound of solids, if zero it is not used
  MODEL_EXPORT virtual void storeModified(const GeomShapePtr& theOldShape,
                                          const GeomShapePtr& theNewShape,
                                          const bool theIsCleanStored = true) override;

  /// Stores the shape without naming support
  /// \param theShape shape to store
  MODEL_EXPORT virtual void storeWithoutNaming(const GeomShapePtr& theShape);

  /// Returns the shape-result produced by this feature
  MODEL_EXPORT virtual GeomShapePtr shape();

  /// Records the subshape newShape which was generated during a topological construction.
  /// As an example, consider the case of a face generated in construction of a box.
  /// Returns true if it is stored correctly (the final shape contains this new sub-shape)
  MODEL_EXPORT virtual bool generated(const GeomShapePtr& theNewShape,
    const std::string& theName, const bool theCheckIsInResult = true) override;

  /// Records the shape newShape which was generated from the shape oldShape during a topological
  /// construction. As an example, consider the case of a face generated from an edge in
  /// construction of a prism.
  MODEL_EXPORT virtual void generated(const GeomShapePtr& theOldShape,
                                      const GeomShapePtr& theNewShape,
                                      const std::string& theName = "") override;


  /// Records the shape newShape which is a modification of the shape oldShape.
  /// As an example, consider the case of a face split or merged in a Boolean operation.
  MODEL_EXPORT virtual void modified(const GeomShapePtr& theOldShape,
                                     const GeomShapePtr& theNewShape,
                                     const std::string& theName = "") override;

  /// Records the shape oldShape which was deleted from the current label.
  /// As an example, consider the case of a face removed by a Boolean operation.
  MODEL_EXPORT virtual void deleted(const GeomShapePtr& theOldShape) override;

  /// load deleted shapes
  MODEL_EXPORT
  virtual void loadDeletedShapes(const GeomMakeShapePtr& theAlgo,
                                 const GeomShapePtr& theOldShape,
                                 const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                 const GeomShapePtr& theShapesToExclude = GeomShapePtr()) override;

  /// load and orient modified shapes
  MODEL_EXPORT
  virtual void loadModifiedShapes(const GeomMakeShapePtr& theAlgo,
                                  const GeomShapePtr& theOldShape,
                                  const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                  const std::string& theName = "") override;

  /// load and orient generated shapes
  MODEL_EXPORT
  virtual void loadGeneratedShapes(const GeomMakeShapePtr& theAlgo,
                                   const GeomShapePtr& theOldShape,
                                   const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                   const std::string& theName = "") override;

  /// Loads shapes of the first level (to be used during shape import)
  MODEL_EXPORT virtual void loadFirstLevel(GeomShapePtr theShape,
                                           const std::string& theName) override;

  /// Loads disconnected edges
  MODEL_EXPORT virtual void loadDisconnectedEdges(GeomShapePtr theShape,
                                                  const std::string& theName) override;

  /// Loads disconnected vetexes
  MODEL_EXPORT virtual void loadDisconnectedVertexes(GeomShapePtr theShape,
                                                     const std::string& theName) override;

  /// Removes the stored builders
  MODEL_EXPORT virtual ~Model_BodyBuilder();

  /// Returns true if the latest modification of this body in the naming history
  // is equal to the given shape
  MODEL_EXPORT virtual bool isLatestEqual(const GeomShapePtr& theShape);

protected:
  /// Default constructor accessible only by Model_Objects
  Model_BodyBuilder(ModelAPI_Object* theOwner);

  /// Removes the stored builders
  void clean();

  /// Returns (creates if necessary) the builder created on the needed tag of sub-label
  TNaming_Builder* builder(const int theTag);

private:
  /// Loads shapes of the next level (to be used during shape import)
  void loadNextLevels(GeomShapePtr theShape,
                      const std::string& theName);

  /// builds name for the shape kept at the specified tag
  void buildName(const int theTag, const std::string& theName);

private:
  struct IndexTags {
    int index;
    std::vector<int> tags;
  };

private:
  int myFreePrimitiveTag;
  std::map<std::string, IndexTags> myPrimitivesNamesIndexMap;

private:
  friend class Model_ResultBody;
  friend class Model_ResultCompSolid;
};

#endif
