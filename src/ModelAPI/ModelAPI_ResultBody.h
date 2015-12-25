// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultBody.hxx
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_ResultBody_H_
#define ModelAPI_ResultBody_H_

#include "ModelAPI_Result.h"
#include <GeomAPI_Shape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <string>

class ModelAPI_BodyBuilder;
class GeomAlgoAPI_MakeShape;

/**\class ModelAPI_ResultBody
* \ingroup DataModel
* \brief The body (shape) result of a feature.
*
* Provides a shape that may be displayed in the viewer.
* May provide really huge results, so, working with this kind
* of result must be optimized.
*/
class ModelAPI_ResultBody : public ModelAPI_Result
{
public:
  MODELAPI_EXPORT virtual ~ModelAPI_ResultBody();
  /// Returns the group identifier of this result
  MODELAPI_EXPORT virtual std::string groupName();

  /// Returns the group identifier of this result
  inline static std::string group()
  {
    static std::string MY_GROUP = "Bodies";
    return MY_GROUP;
  }

  /// default color for a result body
  inline static const std::string& DEFAULT_COLOR()
  {
    static const std::string RESULT_BODY_COLOR("200,200,230");
    return RESULT_BODY_COLOR;
  }

  /// \brief Stores the shape (called by the execution method).
  /// param[in] theShape shape to store.
  /// param[in] theIsStoreSameShapes if false stores reference to the same shape if it is already in document.
  MODELAPI_EXPORT virtual void store(const std::shared_ptr<GeomAPI_Shape>& theShape,
                                     const bool theIsStoreSameShapes = true);

  /// Stores the generated shape (called by the execution method).
  MODELAPI_EXPORT virtual void storeGenerated(const std::shared_ptr<GeomAPI_Shape>& theFromShape,
    const std::shared_ptr<GeomAPI_Shape>& theToShape);

  /// Stores the modified shape (called by the execution method).
  MODELAPI_EXPORT virtual void storeModified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
    const std::shared_ptr<GeomAPI_Shape>& theNewShape, const int theDecomposeSolidsTag = 0);

  /// Stores the shape without naming support
  MODELAPI_EXPORT virtual void storeWithoutNaming(
    const std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Returns the shape-result produced by this feature
  MODELAPI_EXPORT virtual std::shared_ptr<GeomAPI_Shape> shape();

  /// Records the subshape newShape which was generated during a topological construction.
  /// As an example, consider the case of a face generated in construction of a box.
  MODELAPI_EXPORT virtual void generated(const std::shared_ptr<GeomAPI_Shape>& theNewShape,
    const std::string& theName, const int theTag = 1);

  /// Records the shape newShape which was generated from the shape oldShape during a topological 
  /// construction. As an example, consider the case of a face generated from an edge in 
  /// construction of a prism.
  MODELAPI_EXPORT virtual void generated(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
    const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName, 
    const int theTag = 1);

  /// Records the shape newShape which is a modification of the shape oldShape.
  /// As an example, consider the case of a face split or merged in a Boolean operation.
  MODELAPI_EXPORT virtual void modified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
    const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName,
    const int theTag = 1);

  /// Records the shape oldShape which was deleted from the current label.
  /// As an example, consider the case of a face removed by a Boolean operation.
  MODELAPI_EXPORT virtual void deleted(
    const std::shared_ptr<GeomAPI_Shape>& theOldShape, const int theTag = 1);
  
  /// load deleted shapes
  MODELAPI_EXPORT virtual void loadDeletedShapes (GeomAlgoAPI_MakeShape* theMS,
                                  std::shared_ptr<GeomAPI_Shape>  theShapeIn,
                                  const int  theKindOfShape,
                                  const int  theTag);
  /// load and orient modified shapes
  MODELAPI_EXPORT virtual void loadAndOrientModifiedShapes (GeomAlgoAPI_MakeShape* theMS,
    std::shared_ptr<GeomAPI_Shape>  theShapeIn, const int  theKindOfShape, const int  theTag,
    const std::string& theName, GeomAPI_DataMapOfShapeShape& theSubShapes, const bool theIsStoreSeparate = false);
  /// load and orient generated shapes
  MODELAPI_EXPORT virtual void loadAndOrientGeneratedShapes (GeomAlgoAPI_MakeShape* theMS,
    std::shared_ptr<GeomAPI_Shape>  theShapeIn, const int  theKindOfShape,
    const int  theTag, const std::string& theName, GeomAPI_DataMapOfShapeShape& theSubShapes);

  /// load shapes of the first level (to be used during shape import)
  MODELAPI_EXPORT virtual void loadFirstLevel(std::shared_ptr<GeomAPI_Shape> theShape, 
    const std::string& theName, int&  theTag);
  
  /// load disconnected edges
  MODELAPI_EXPORT virtual void loadDisconnectedEdges(std::shared_ptr<GeomAPI_Shape> theShape,
    const std::string& theName, int&  theTag);

  /// load disconnected vetexes
  MODELAPI_EXPORT virtual void loadDisconnectedVertexes(std::shared_ptr<GeomAPI_Shape> theShape,
    const std::string& theName,int&  theTag);

  /// Returns true if the latest modification of this body in the naming history
  // is equal to the given shape
  MODELAPI_EXPORT virtual bool isLatestEqual(const std::shared_ptr<GeomAPI_Shape>& theShape) = 0;

protected:
  /// Default constructor accessible only from Model_Objects
  MODELAPI_EXPORT ModelAPI_ResultBody();

  ModelAPI_BodyBuilder* myBuilder; ///< provides the body processing in naming shape
};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultBody> ResultBodyPtr;

#endif
