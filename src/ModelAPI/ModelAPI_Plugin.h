// File:        ModelAPI_Plugin.hxx
// Created:     31 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Plugin_HeaderFile
#define ModelAPI_Plugin_HeaderFile

#include "ModelAPI.h"
#include <string>
#include <boost/shared_ptr.hpp>

class ModelAPI_Feature;

/**\class ModelAPI_Plugin
 * \ingroup DataModel
 * \brief Interface common for any plugin: allows to use plugin by the plugins manager.
 */

class MODELAPI_EXPORT ModelAPI_Plugin
{
public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual boost::shared_ptr<ModelAPI_Feature> createFeature(std::string theFeatureID) = 0;

protected:
  /// Is needed for python wrapping by swig
  ModelAPI_Plugin() {};
};

#endif
