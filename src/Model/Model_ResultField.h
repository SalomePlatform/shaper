// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultField.h
// Created:     16 Nov 2016
// Author:      Mikhail PONIKAROV

#ifndef Model_ResultField_H_
#define Model_ResultField_H_

#include "Model.h"
#include <ModelAPI_ResultField.h>

/**\class Model_ResultField
 * \ingroup DataModel
 * \brief The fields result.
 *
 * Provides a compound of selected elements, without storage, one the fly.
 */
class Model_ResultField : public ModelAPI_ResultField
{
  std::shared_ptr<ModelAPI_Data> myOwnerData;  ///< data of owner of this result
public:

  /// Retuns the parameters of color definition in the resources config manager
  MODEL_EXPORT virtual void colorConfigInfo(std::string& theSection, std::string& theName,
                                            std::string& theDefault);

  /// Returns the compound of selected entities
  MODEL_EXPORT virtual std::shared_ptr<GeomAPI_Shape> shape();

  /// Removes the stored builders
  MODEL_EXPORT virtual ~Model_ResultField() {}

protected:
  /// Makes a body on the given feature data
  Model_ResultField(std::shared_ptr<ModelAPI_Data> theOwnerData);

  friend class Model_Objects;
};

#endif
