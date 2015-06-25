// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultBody.hxx
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_ResultBody_H_
#define ModelAPI_ResultBody_H_

#include "ModelAPI_Result.h"
#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <memory>
#include <string>

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

  /// Stores the shape (called by the execution method).
  virtual void store(const std::shared_ptr<GeomAPI_Shape>& theShape) = 0;

  /// Stores the generated shape (called by the execution method).
  virtual void storeGenerated(const std::shared_ptr<GeomAPI_Shape>& theFromShape,
	                          const std::shared_ptr<GeomAPI_Shape>& theToShape) = 0;

  /// Stores the modified shape (called by the execution method).
  virtual void storeModified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
	                          const std::shared_ptr<GeomAPI_Shape>& theNewShape,
                            const int theDecomposeSolidsTag = 0) = 0;

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
                                               GeomAPI_DataMapOfShapeShape& theSubShapes) = 0;
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

protected:
};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultBody> ResultBodyPtr;

#endif
