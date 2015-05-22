// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultGroup.h
// Created:     08 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_ResultGroup_H_
#define Model_ResultGroup_H_

#include "Model.h"
#include <ModelAPI_ResultGroup.h>

/**\class Model_ResultGroup
 * \ingroup DataModel
 * \brief The groups result.
 *
 * Provides a compound of selected elements, without storage, one the fly.
 */
class Model_ResultGroup : public ModelAPI_ResultGroup
{
  std::shared_ptr<ModelAPI_Data> myOwnerData;  ///< data of owner of this result
public:
  /// default color for a result body
  inline static const std::string& DEFAULT_COLOR()
  {
    static const std::string RESULT_GROUP_COLOR("150,150,180");
    return RESULT_GROUP_COLOR;
  }
  /// Request for initialization of data model of the result: adding all attributes
  virtual void initAttributes();

  // Retuns the parameters of color definition in the resources config manager
  MODEL_EXPORT virtual void colorConfigInfo(std::string& theSection, std::string& theName,
                                            std::string& theDefault);

  /// Returns the compound of selected entities
  MODEL_EXPORT virtual std::shared_ptr<GeomAPI_Shape> shape();

  /// Removes the stored builders
  MODEL_EXPORT virtual ~Model_ResultGroup() {}

protected:
  /// Makes a body on the given feature data
  Model_ResultGroup(std::shared_ptr<ModelAPI_Data> theOwnerData);

  friend class Model_Objects;
};

#endif
