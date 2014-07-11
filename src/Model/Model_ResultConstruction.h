// File:        ModelAPI_ResultConstruction.hxx
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_ResultConstruction_HeaderFile
#define Model_ResultConstruction_HeaderFile

#include "Model.h"
#include <ModelAPI_ResultConstruction.h>

/**\class ModelAPI_ResultConstruction
 * \ingroup DataModel
 * \brief The construction element result of a feature.
 *
 * Provides a shape that may be displayed in the viewer.
 * Intermediate, light result that in many cases produces a result on the fly.
 */
class Model_ResultConstruction : public ModelAPI_ResultConstruction
{
  boost::shared_ptr<ModelAPI_Feature> myOwner; ///< owner of this result
  boost::shared_ptr<GeomAPI_Shape> myShape; ///< shape of this result created "on the fly"
public:
  /// Returns the group identifier of this result
  virtual std::string group()
    {static std::string MY_GROUP = "Construction"; return MY_GROUP;}
  /// Sets the result
  MODEL_EXPORT virtual void setShape(boost::shared_ptr<GeomAPI_Shape> theShape);
  /// Returns the shape-result produced by this feature
  MODEL_EXPORT virtual boost::shared_ptr<GeomAPI_Shape>& shape();
  /// Returns the source feature of this result
  //MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Feature> owner();

protected:
  /// Makes a body on the given feature
  Model_ResultConstruction();

  friend class Model_Document;
};

#endif
