// File:        Model_Feature.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Feature_HeaderFile
#define Model_Feature_HeaderFile

#include "Model.hxx"
#include <ModelAPI_Feature.hxx>

/**\class Model_Feature
 * \ingroup DataModel
 * \brief General object of the application that allows
 * to get/set attributes from the document and compute result of an operation.
 */

class Model_Feature: public ModelAPI_Feature
{
  Model_Feature();

  friend class Model_PluginManager;
public:
  /// Returns the kind of a feature (like "Point")
  virtual std::string GetKind();
};

#endif
