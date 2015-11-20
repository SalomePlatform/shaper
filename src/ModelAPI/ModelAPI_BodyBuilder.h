// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_BodyBuilder.hxx
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

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
  virtual void store(const std::shared_ptr<GeomAPI_Shape>& theShape) = 0;

  /// Stores the generated shape (called by the execution method).
  virtual void storeGenerated(const std::shared_ptr<GeomAPI_Shape>& theFromShape,
	                          const std::shared_ptr<GeomAPI_Shape>& theToShape) = 0;

  /// Stores the modified shape (called by the execution method).
  virtual void storeModified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
	                          const std::shared_ptr<GeomAPI_Shape>& theNewShape,
                            const int theDecomposeSolidsTag = 0) = 0;

  /// Stores the shape without naming support
  virtual void storeWithoutNaming(const std::shared_ptr<GeomAPI_Shape>& theShape) = 0;

  /// Returns the shape-result produced by this feature
  virtual std::shared_ptr<GeomAPI_Shape> shape() = 0;

  /// Records the subshape newShape which was generated during a topological construction.
  /// As an example, consider the case of a face generated in construction of a box.
  virtual void generated(
    const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName, const int theTag = 1) = 0;

  /// Records the shape newShape which was generated from the shape oldShape during a topological 
  /// construction. As an example, consider the case of a face generated from an edge in 
  /// construction of a prism.
  virtual void generated(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
    const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName, const int theTag = 1) = 0;

  /// Records the shape newShape which is a modification of the shape oldShape.
  /// As an example, consider the case of a face split or merged in a Boolean operation.
  virtual void modified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
    const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName, const int theTag = 1) = 0;

  /// Records the shape oldShape which was deleted from the current label.
  /// As an example, consider the case of a face removed by a Boolean operation.
  virtual void deleted(
    const std::shared_ptr<GeomAPI_Shape>& theOldShape, const int theTag = 1) = 0;
  
  /// load deleted shapes
  virtual void loadDeletedShapes (GeomAlgoAPI_MakeShape* theMS,
                                               std::shared_ptr<GeomAPI_Shape>  theShapeIn,
                                               const int  theKindOfShape,
                                               const int  theTag) = 0;
  /// load and orient modified shapes
  virtual void loadAndOrientModifiedShapes (
	                                           GeomAlgoAPI_MakeShape* theMS,
                                               std::shared_ptr<GeomAPI_Shape>  theShapeIn,
                                               const int  theKindOfShape,
                                               const int  theTag,
											   const std::string& theName,
                                               GeomAPI_DataMapOfShapeShape& theSubShapes,
                                               const bool theIsStoreSeparate = false) = 0;
  /// load and orient generated shapes
  virtual void loadAndOrientGeneratedShapes (
	                                           GeomAlgoAPI_MakeShape* theMS,
                                               std::shared_ptr<GeomAPI_Shape>  theShapeIn,
                                               const int  theKindOfShape,
                                               const int  theTag,
											   const std::string& theName,
                                               GeomAPI_DataMapOfShapeShape& theSubShapes) = 0;

  /// load shapes of the first level (to be used during shape import)
  virtual void loadFirstLevel(std::shared_ptr<GeomAPI_Shape> theShape, const std::string& theName, int&  theTag) = 0;
  
  /// load disconnected edges
  virtual void loadDisconnectedEdges(std::shared_ptr<GeomAPI_Shape> theShape, const std::string& theName, int&  theTag) = 0;

  /// load disconnected vetexes
  virtual void loadDisconnectedVertexes(std::shared_ptr<GeomAPI_Shape> theShape, const std::string& theName,int&  theTag) = 0;

  /// Converts evolution of sub-shapes stored in naming structure to selection 
  /// (theFlag = true) and back (theFlag = false)
  virtual void evolutionToSelection(const bool theFlag) = 0;

  /// Returns true if the latest modification of this body in the naming history
  // is equal to the given shape
  virtual bool isLatestEqual(const std::shared_ptr<GeomAPI_Shape>& theShape) = 0;

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
