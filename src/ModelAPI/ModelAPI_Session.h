// File:        ModelAPI_Session.hxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Session_H_
#define ModelAPI_Session_H_

#include "ModelAPI.h"
#include <string>
#include <boost/shared_ptr.hpp>

class ModelAPI_Feature;
class ModelAPI_Plugin;
class ModelAPI_Document;
class ModelAPI_ValidatorsFactory;

/**\class ModelAPI_Session
 * \ingroup DataModel
 * \brief Object that knows (from the initial XML file) which
 * plugin contains which feature, loads and stores reference to loaded plugins by
 * the feature functionality request.
 */

class MODELAPI_EXPORT ModelAPI_Session
{
 public:
  /// Returns the real implementation (the alone instance per application) of the plugin manager
  static boost::shared_ptr<ModelAPI_Session> get();

  /// Registers the plugin that creates features.
  /// It is obligatory for each plugin to call this function on loading to be found by 
  /// the plugin manager on call of the feature)
  virtual void registerPlugin(ModelAPI_Plugin* thePlugin) = 0;

  /// Returns the root document of the application (that may contains sub-documents)
  virtual boost::shared_ptr<ModelAPI_Document> rootDocument() = 0;

  /// Return true if root document has been already created
  virtual bool hasRootDocument() = 0;

  /// Returns the current document that used for current work in the application
  virtual boost::shared_ptr<ModelAPI_Document> currentDocument() = 0;

  /// Defines the current document that used for current work in the application
  virtual void setCurrentDocument(boost::shared_ptr<ModelAPI_Document> theDoc) = 0;

  /// Copies the document to the new one with the given id
  virtual boost::shared_ptr<ModelAPI_Document> copy(boost::shared_ptr<ModelAPI_Document> theSource,
                                                    std::string theID) = 0;

  /// Returns the validators factory: the only one instance per application
  virtual ModelAPI_ValidatorsFactory* validators() = 0;

  /// Is needed for python wrapping by swig, call Get to get an instance
  ModelAPI_Session();

  /// To virtually destroy the fields of successors
  virtual ~ModelAPI_Session()
  {
  }

 protected:
  /// Creates the feature object using plugins functionality
  virtual boost::shared_ptr<ModelAPI_Feature> createFeature(std::string theFeatureID) = 0;

  static void setSession(boost::shared_ptr<ModelAPI_Session> theManager);

  friend class Model_Document;
};

typedef boost::shared_ptr<ModelAPI_Session> SessionPtr;

#endif
