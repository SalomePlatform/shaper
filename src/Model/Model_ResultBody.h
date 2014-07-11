// File:        Model_ResultBody.h
// Created:     08 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_ResultBody_HeaderFile
#define Model_ResultBody_HeaderFile

#include "Model.h"
#include <ModelAPI_ResultBody.h>

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
  boost::shared_ptr<ModelAPI_Feature> myOwner; ///< owner of this result
public:
  /// Stores the shape (called by the execution method).
  MODEL_EXPORT virtual void store(const boost::shared_ptr<GeomAPI_Shape>& theShape);
  /// Returns the shape-result produced by this feature
  MODEL_EXPORT virtual boost::shared_ptr<GeomAPI_Shape> shape();
  /// Returns the source feature of this result
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Feature> owner();

protected:
  /// Makes a body on the given feature
  Model_ResultBody(const boost::shared_ptr<ModelAPI_Feature>& theFeature);

  friend class Model_Document;
};

#endif
