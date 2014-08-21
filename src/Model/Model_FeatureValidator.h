// File:        ModelAPI_FeatureValidator.h
// Created:     8 Jul 2014
// Author:      Vitaly SMETANNIKOV

#ifndef Model_FeatureValidator_H
#define Model_FeatureValidator_H

#include <Model.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_FeatureValidator.h>

#include <boost/shared_ptr.hpp>

class Model_FeatureValidator : public ModelAPI_FeatureValidator
{
 public:
  /// Returns true if feature and/or attributes are valid
  /// \param theFeature the validated feature
  MODEL_EXPORT virtual bool isValid(const boost::shared_ptr<ModelAPI_Feature>& theFeature) const;
};

#endif
