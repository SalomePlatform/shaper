// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultBody.h
// Created:     08 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_ResultBody_H_
#define Model_ResultBody_H_

#include "Model.h"
#include <ModelAPI_ResultBody.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <vector>

class TNaming_Builder;

/**\class Model_ResultBody
 * \ingroup DataModel
 * \brief The body (shape) result of a feature.
 *
 * Provides a shape that may be displayed in the viewer.
 * May provide really huge results, so, working with this kind
 * of result must be optimized.
 */
class Model_ResultBody : public ModelAPI_ResultBody
{
  /// builders that tores the naming history: one per label to allow store several shapes to one 
  /// label; index in vector corresponds to the label tag
  std::vector<TNaming_Builder*> myBuilders;
public:
  /// Request for initialization of data model of the result: adding all attributes
  virtual void initAttributes();

  /// Stores the shape (called by the execution method).
  MODEL_EXPORT virtual void store(const std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Stores the generated shape (called by the execution method).
  MODEL_EXPORT virtual void storeGenerated(const std::shared_ptr<GeomAPI_Shape>& theFromShape,
	                                       const std::shared_ptr<GeomAPI_Shape>& theToShape);

  /// Stores the modified shape (called by the execution method).
  MODEL_EXPORT virtual void storeModified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
	                                       const std::shared_ptr<GeomAPI_Shape>& theNewShape);

  /// Returns the shape-result produced by this feature
  MODEL_EXPORT virtual std::shared_ptr<GeomAPI_Shape> shape();

  /// Records the subshape newShape which was generated during a topological construction.
  /// As an example, consider the case of a face generated in construction of a box.
  MODEL_EXPORT virtual void generated(const std::shared_ptr<GeomAPI_Shape>& theNewShape, 
    const std::string& theName, const int theTag = 1);

  /// Records the shape newShape which was generated from the shape oldShape during a topological 
  /// construction. As an example, consider the case of a face generated from an edge in 
  /// construction of a prism.
  MODEL_EXPORT virtual void generated(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
    const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName, const int theTag = 1);


  /// Records the shape newShape which is a modification of the shape oldShape.
  /// As an example, consider the case of a face split or merged in a Boolean operation.
  MODEL_EXPORT virtual void modified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
    const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName, const int theTag = 1);

  /// Records the shape oldShape which was deleted from the current label.
  /// As an example, consider the case of a face removed by a Boolean operation.
  MODEL_EXPORT virtual void deleted(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
    const int theTag = 1);

  /// load deleted shapes
  MODEL_EXPORT virtual void loadDeletedShapes (GeomAlgoAPI_MakeShape* theMS,
                                               std::shared_ptr<GeomAPI_Shape>  theShapeIn,
                                               const int  theKindOfShape,
                                               const int  theTag);
  /// load and orient modified shapes
  MODEL_EXPORT virtual void loadAndOrientModifiedShapes (
	                                           GeomAlgoAPI_MakeShape* theMS,
                                               std::shared_ptr<GeomAPI_Shape>  theShapeIn,
                                               const int  theKindOfShape,
                                               const int  theTag,
											   const std::string& theName,
                                               GeomAPI_DataMapOfShapeShape& theSubShapes);
   /// load and orient generated shapes
  MODEL_EXPORT virtual void loadAndOrientGeneratedShapes (
	                                           GeomAlgoAPI_MakeShape* theMS,
                                               std::shared_ptr<GeomAPI_Shape>  theShapeIn,
                                               const int  theKindOfShape,
                                               const int  theTag,
											   const std::string& theName,
                                               GeomAPI_DataMapOfShapeShape& theSubShapes);
  
  /// Loads shapes of the first level (to be used during shape import)
  MODEL_EXPORT virtual void loadFirstLevel(std::shared_ptr<GeomAPI_Shape> theShape, const std::string& theName, int&  theTag);
  
  /// Loads disconnected edges
  MODEL_EXPORT virtual void loadDisconnectedEdges(std::shared_ptr<GeomAPI_Shape> theShape, const std::string& theName, int&  theTag);

  /// Loads disconnected vetexes
  MODEL_EXPORT virtual void loadDisconnectedVertexes(std::shared_ptr<GeomAPI_Shape> theShape, const std::string& theName, int&  theTag);

  /// Removes the stored builders
  MODEL_EXPORT virtual ~Model_ResultBody();

protected:
  /// Makes a body on the given feature
  Model_ResultBody();

  /// Removes the stored builders
  void clean();

  /// Returns (creates if necessary) the builder created on the needed tag of sub-label
  TNaming_Builder* builder(const int theTag);

private:
  /// Loads shapes of the next level (to be used during shape import)
  void loadNextLevels(std::shared_ptr<GeomAPI_Shape> theShape, const std::string& theName, int&  theTag);

  /// builds name for the shape kept at the specified tag 
  void buildName(const int theTag, const std::string& theName);

  friend class Model_Document;
};

#endif
