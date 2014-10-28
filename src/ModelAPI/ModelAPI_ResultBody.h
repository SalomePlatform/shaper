// File:        ModelAPI_ResultBody.hxx
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_ResultBody_H_
#define ModelAPI_ResultBody_H_

#include "ModelAPI_Result.h"
#include <GeomAPI_Shape.h>

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
  /// Returns the group identifier of this result
  virtual std::string groupName()
  {
    return group();
  }

  /// Returns the group identifier of this result
  static std::string group()
  {
    static std::string MY_GROUP = "Bodies";
    return MY_GROUP;
  }

  /// Stores the shape (called by the execution method).
  virtual void store(const boost::shared_ptr<GeomAPI_Shape>& theShape) = 0;

  /// Stores the generated shape (called by the execution method).
  virtual void storeGenerated(const boost::shared_ptr<GeomAPI_Shape>& theFromShape,
	                          const boost::shared_ptr<GeomAPI_Shape>& theToShape) = 0;

  /// Stores the modified shape (called by the execution method).
  virtual void storeModified(const boost::shared_ptr<GeomAPI_Shape>& theOldShape,
	                          const boost::shared_ptr<GeomAPI_Shape>& theNewShape) = 0;

  /// Returns the shape-result produced by this feature
  virtual boost::shared_ptr<GeomAPI_Shape> shape() = 0;

  /// Records the subshape newShape which was generated during a topological construction.
  /// As an example, consider the case of a face generated in construction of a box.
  virtual void generated(
    const boost::shared_ptr<GeomAPI_Shape>& theNewShape, const int theTag = 1) = 0;

  /// Records the shape newShape which was generated from the shape oldShape during a topological 
  /// construction. As an example, consider the case of a face generated from an edge in 
  /// construction of a prism.
  virtual void generated(const boost::shared_ptr<GeomAPI_Shape>& theOldShape,
    const boost::shared_ptr<GeomAPI_Shape>& theNewShape, const int theTag = 1) = 0;

  /// Records the shape newShape which is a modification of the shape oldShape.
  /// As an example, consider the case of a face split or merged in a Boolean operation.
  virtual void modified(const boost::shared_ptr<GeomAPI_Shape>& theOldShape,
    const boost::shared_ptr<GeomAPI_Shape>& theNewShape, const int theTag = 1) = 0;

  /// Records the shape oldShape which was deleted from the current label.
  /// As an example, consider the case of a face removed by a Boolean operation.
  virtual void deleted(
    const boost::shared_ptr<GeomAPI_Shape>& theOldShape, const int theTag = 1) = 0;

protected:
};

//! Pointer on feature object
typedef boost::shared_ptr<ModelAPI_ResultBody> ResultBodyPtr;

#endif
