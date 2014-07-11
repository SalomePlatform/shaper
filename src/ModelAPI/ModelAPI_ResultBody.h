// File:        ModelAPI_ResultBody.hxx
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_ResultBody_HeaderFile
#define ModelAPI_ResultBody_HeaderFile

#include "ModelAPI_Result.h"
#include <GeomAPI_Shape.h>

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
  virtual std::string group()
    {static std::string MY_GROUP = "Bodies"; return MY_GROUP;}

  /// Stores the shape (called by the execution method).
  virtual void store(const boost::shared_ptr<GeomAPI_Shape>& theShape) = 0;
  /// Returns the shape-result produced by this feature
  virtual boost::shared_ptr<GeomAPI_Shape> shape() = 0;

  /// To virtually destroy the fields of successors
  virtual ~ModelAPI_ResultBody() {}

protected:
  /// Use plugin manager for features creation: this method is 
  /// defined here only for SWIG-wrapping
  ModelAPI_ResultBody()
  {}
};

//! Pointer on feature object
typedef boost::shared_ptr<ModelAPI_ResultBody> ResultBodyPtr;

#endif
