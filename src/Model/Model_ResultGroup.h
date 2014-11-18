// File:        Model_ResultGroup.h
// Created:     08 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_ResultGroup_H_
#define Model_ResultGroup_H_

#include "Model.h"
#include <ModelAPI_ResultGroup.h>

/**\class ModelAPI_ResultGroup
 * \ingroup DataModel
 * \brief The groups result.
 *
 * Provides a compound of selected elements, without storage, one the fly.
 */
class Model_ResultGroup : public ModelAPI_ResultGroup
{
  boost::shared_ptr<ModelAPI_Data> myOwnerData;  ///< data of owner of this result
public:
  /// Returns the compound of selected entities
  MODEL_EXPORT virtual boost::shared_ptr<GeomAPI_Shape> shape();

  /// Removes the stored builders
  MODEL_EXPORT virtual ~Model_ResultGroup() {}

protected:
  /// Makes a body on the given feature data
  Model_ResultGroup(boost::shared_ptr<ModelAPI_Data> theOwnerData);

  friend class Model_Document;
};

#endif
