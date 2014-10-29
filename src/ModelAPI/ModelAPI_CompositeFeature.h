// File:        ModelAPI_CompositeFeature.hxx
// Created:     20 Oct 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_CompositeFeature_H_
#define ModelAPI_CompositeFeature_H_

#include "ModelAPI_Feature.h"

/**\class ModelAPI_CompositeFeature
 * \ingroup DataModel
 * \brief Feature that consists of other features: like sketcher
 * with edges inside. It just allows t oadd a feature to this feature only
 * instead of adding in both document and this feature.
 */
class ModelAPI_CompositeFeature : public ModelAPI_Feature
{
public:
  /// Adds feature to the sketch and to its document
  virtual boost::shared_ptr<ModelAPI_Feature> addFeature(std::string theID) = 0;

  /// Returns the number of sub-elements
  virtual int numberOfSubs() const = 0;

  /// Returns the sub-feature by zero-base index
  virtual boost::shared_ptr<ModelAPI_Feature> subFeature(const int theIndex) const = 0;

  /// Returns the sub-feature unique identifier in this composite feature by zero-base index
  virtual int subFeatureId(const int theIndex) const = 0;

  /// Returns true if feature or reuslt belong to this composite feature as subs
  virtual bool isSub(ObjectPtr theObject) const = 0;
};

//! Pointer on the composite feature object
typedef boost::shared_ptr<ModelAPI_CompositeFeature> CompositeFeaturePtr;

#endif
