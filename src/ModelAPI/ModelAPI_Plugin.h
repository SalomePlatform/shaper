// File:        ModelAPI_Plugin.hxx
// Created:     31 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Plugin_H_
#define ModelAPI_Plugin_H_

#include "ModelAPI.h"
#include <string>
#include <memory>

class ModelAPI_Feature;

/**\class ModelAPI_Plugin
 * \ingroup DataModel
 * \brief Interface common for any plugin: allows to use plugin by the plugins manager.
 */

class MODELAPI_EXPORT ModelAPI_Plugin
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual std::shared_ptr<ModelAPI_Feature> createFeature(std::string theFeatureID) = 0;

  /// To virtually destroy the fields of successors
  virtual ~ModelAPI_Plugin()
  {
  }
};

#endif
