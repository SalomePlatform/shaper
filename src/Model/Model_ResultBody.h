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

/**\class ModelAPI_ResultBody
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
  /// Stores the shape (called by the execution method).
  MODEL_EXPORT virtual void store(const boost::shared_ptr<GeomAPI_Shape>& theShape);

  /// Stores the generated shape (called by the execution method).
  MODEL_EXPORT virtual void storeGenerated(const boost::shared_ptr<GeomAPI_Shape>& theFromShape,
	                                       const boost::shared_ptr<GeomAPI_Shape>& theToShape);

  /// Stores the modified shape (called by the execution method).
  MODEL_EXPORT virtual void storeModified(const boost::shared_ptr<GeomAPI_Shape>& theOldShape,
	                                       const boost::shared_ptr<GeomAPI_Shape>& theNewShape);

  /// Returns the shape-result produced by this feature
  MODEL_EXPORT virtual boost::shared_ptr<GeomAPI_Shape> shape();

  /// Records the subshape newShape which was generated during a topological construction.
  /// As an example, consider the case of a face generated in construction of a box.
  MODEL_EXPORT virtual void generated(const boost::shared_ptr<GeomAPI_Shape>& theNewShape, 
    const int theTag = 1);

  /// Records the shape newShape which was generated from the shape oldShape during a topological 
  /// construction. As an example, consider the case of a face generated from an edge in 
  /// construction of a prism.
  MODEL_EXPORT virtual void generated(const boost::shared_ptr<GeomAPI_Shape>& theOldShape,
    const boost::shared_ptr<GeomAPI_Shape>& theNewShape, const int theTag = 1);


  /// Records the shape newShape which is a modification of the shape oldShape.
  /// As an example, consider the case of a face split or merged in a Boolean operation.
  MODEL_EXPORT virtual void modified(const boost::shared_ptr<GeomAPI_Shape>& theOldShape,
    const boost::shared_ptr<GeomAPI_Shape>& theNewShape, const int theTag = 1);

  /// Records the shape oldShape which was deleted from the current label.
  /// As an example, consider the case of a face removed by a Boolean operation.
  MODEL_EXPORT virtual void deleted(const boost::shared_ptr<GeomAPI_Shape>& theOldShape,
    const int theTag = 1);

  /// load deleted shapes
  MODEL_EXPORT virtual void loadDeletedShapes (GeomAlgoAPI_MakeShape* theMS,
                                               boost::shared_ptr<GeomAPI_Shape>  theShapeIn,
                                               const int  theKindOfShape,
                                               const int  theTag);
  /// load and orient modified shapes
  MODEL_EXPORT virtual void loadAndOrientModifiedShapes (
	                                           GeomAlgoAPI_MakeShape* theMS,
                                               boost::shared_ptr<GeomAPI_Shape>  theShapeIn,
                                               const int  theKindOfShape,
                                               const int  theTag,
                                               GeomAPI_DataMapOfShapeShape& theSubShapes);
   /// load and orient generated shapes
  MODEL_EXPORT virtual void loadAndOrientGeneratedShapes (
	                                           GeomAlgoAPI_MakeShape* theMS,
                                               boost::shared_ptr<GeomAPI_Shape>  theShapeIn,
                                               const int  theKindOfShape,
                                               const int  theTag,
                                               GeomAPI_DataMapOfShapeShape& theSubShapes);

  /// Removes the stored builders
  MODEL_EXPORT virtual ~Model_ResultBody();

protected:
  /// Makes a body on the given feature
  Model_ResultBody();

  /// Removes the stored builders
  void clean();

  /// Returns (creates if necessary) the builder created on the needed tag of sub-label
  TNaming_Builder* builder(const int theTag);

  friend class Model_Document;
};

#endif
