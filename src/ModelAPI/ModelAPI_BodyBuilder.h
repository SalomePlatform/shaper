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

#ifndef ModelAPI_BodyBuilder_H_
#define ModelAPI_BodyBuilder_H_

#include <ModelAPI.h>
#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <memory>
#include <string>

class ModelAPI_Data;
class ModelAPI_Document;
class ModelAPI_Object;

/**\class ModelAPI_BodyBuilder
* \ingroup DataModel
 * \brief Extra API for the ResultBody that allows to work with naming.
*/
class ModelAPI_BodyBuilder
{
public:
  MODELAPI_EXPORT virtual ~ModelAPI_BodyBuilder() {};

  /// Stores the shape (called by the execution method).
  virtual void store(const GeomShapePtr& theShape,
                     const bool theIsStoreSameShapes = true) = 0;

  /// Stores the generated shape (called by the execution method).
  virtual void storeGenerated(const GeomShapePtr& theFromShape,
                              const GeomShapePtr& theToShape) = 0;

  /// Stores the modified shape (called by the execution method).
  virtual void storeModified(const GeomShapePtr& theOldShape,
                             const GeomShapePtr& theNewShape,
                             const bool theIsCleanStored = true) = 0;

  /// Stores the shape without naming support
  virtual void storeWithoutNaming(const GeomShapePtr& theShape) = 0;

  /// Returns the shape-result produced by this feature
  virtual GeomShapePtr shape() = 0;

  /// Records the subshape newShape which was generated during a topological construction.
  /// As an example, consider the case of a face generated in construction of a box.
  virtual void generated(const GeomShapePtr& theNewShape,
                         const std::string& theName) = 0;

  /// Records the shape newShape which was generated from the shape oldShape during a topological
  /// construction. As an example, consider the case of a face generated from an edge in
  /// construction of a prism.
  virtual void generated(const GeomShapePtr& theOldShape,
                         const GeomShapePtr& theNewShape,
                         const std::string& theName = "") = 0;

  /// Records the shape newShape which is a modification of the shape oldShape.
  /// As an example, consider the case of a face split or merged in a Boolean operation.
  virtual void modified(const GeomShapePtr& theOldShape,
                        const GeomShapePtr& theNewShape,
                        const std::string& theName = "") = 0;

  /// Records the shape oldShape which was deleted from the current label.
  /// As an example, consider the case of a face removed by a Boolean operation.
  virtual void deleted(const GeomShapePtr& theOldShape) = 0;

  /// load deleted shapes
  virtual void loadDeletedShapes(const GeomMakeShapePtr& theAlgo,
                                 const GeomShapePtr& theOldShape,
                                 const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                 const GeomShapePtr& theShapesToAvoid = GeomShapePtr()) = 0;

  /// load and orient modified shapes
  virtual void loadModifiedShapes(const GeomMakeShapePtr& theAlgo,
                                  const GeomShapePtr& theOldShape,
                                  const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                  const std::string& theName = "") = 0;

  /// load and orient generated shapes
  virtual void loadGeneratedShapes(const GeomMakeShapePtr& theAlgo,
                                   const GeomShapePtr& theOldShape,
                                   const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                   const std::string& theName = "") = 0;

  /// load shapes of the first level (to be used during shape import)
  virtual void loadFirstLevel(GeomShapePtr theShape,
                              const std::string& theName) = 0;

  /// load disconnected edges
  virtual void loadDisconnectedEdges(GeomShapePtr theShape,
                                     const std::string& theName) = 0;

  /// load disconnected vetexes
  virtual void loadDisconnectedVertexes(GeomShapePtr theShape,
                                        const std::string& theName) = 0;

  /// Returns true if the latest modification of this body in the naming history
  // is equal to the given shape
  virtual bool isLatestEqual(const GeomShapePtr& theShape) = 0;

protected:
  /// Returns the data manager of this object: attributes
  MODELAPI_EXPORT virtual std::shared_ptr<ModelAPI_Data> data() const;

  /// Returns document this feature belongs to
  MODELAPI_EXPORT virtual std::shared_ptr<ModelAPI_Document> document() const;

protected:
  /// Deafult constructor accessible only from Model_Object
  MODELAPI_EXPORT ModelAPI_BodyBuilder(ModelAPI_Object* theOwner);

  ModelAPI_Object* myOwner; ///< the owner object this builder belongs to
};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_BodyBuilder> BodyBuilderPtr;

#endif
