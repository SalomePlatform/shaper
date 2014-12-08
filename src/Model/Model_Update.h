// File:        Model_Update.hxx
// Created:     25 Jun 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Update_H_
#define Model_Update_H_

#include "Model.h"
#include <ModelAPI_Data.h>
#include "Events_Listener.h"
#include <memory>
#include <set>
#include <map>

class ModelAPI_Object;
class ModelAPI_Feature;

/**\class Model_Update
 * \ingroup DataModel
 * \brief Updates the results of features when it is needed.
 */
class Model_Update : public Events_Listener
{
  /// initial set of updated features that must be processed
  std::set<std::shared_ptr<ModelAPI_Object> > myInitial;
  /// already updated and processed features and modificated feature flag
  std::map<std::shared_ptr<ModelAPI_Object>, bool> myUpdated;
  /// to know that all next updates are caused by this execution
  bool isExecuted;
  /// to know execute or not automatically all update
  bool isAutomatic;
  /// just created features: they must be updated immideately even in not-automatic mode for 
  /// preview; cleared on commit operations
  std::set<std::shared_ptr<ModelAPI_Object> > myJustCreatedOrUpdated;

 public:
  /// Is called only once, on startup of the application
  Model_Update();

  /// Processes the feature argument update: executes the results
  MODEL_EXPORT virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

protected:
  /// updates all features in the document and then - in sub-documents
  void updateInDoc(std::shared_ptr<ModelAPI_Document> theDoc);
  /// Recoursively checks and updates the feature if needed (calls the execute method)
  /// Returns true if feature was updated.
  bool updateFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// Recoursively checks and updates the object (result or feature) if needed (calls updateFeature)
  /// Returns true if object was updated.
  bool updateObject(std::shared_ptr<ModelAPI_Object> theObject, const bool theCyclic = true);
  /// Sends the redisplay events for feature and results, updates the updated status
  void redisplayWithResults(std::shared_ptr<ModelAPI_Feature> theFeature, 
    const ModelAPI_ExecState theState);
};

#endif
