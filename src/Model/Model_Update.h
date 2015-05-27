// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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
  /// created features during this transaction: must be updated all the time
  std::set<std::shared_ptr<ModelAPI_Object> > myJustCreated;
  /// updated features during this transaction: must be updated in the end of transaction
  std::set<std::shared_ptr<ModelAPI_Object> > myJustUpdated;
  /// to know that all next updates are caused by this execution
  bool myIsExecuted;
  /// to know execute or not automatically all update
  bool myIsAutomatic;
  /// to know that some parameter was changed during this operation
  bool myIsParamUpdated;

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
  void updateFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// Updates the selection and parametrical arguments before the later feature analysis
  void updateArguments(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// Sends the redisplay events for feature and results, updates the updated status
  void redisplayWithResults(std::shared_ptr<ModelAPI_Feature> theFeature, 
    const ModelAPI_ExecState theState);

  /// On operation start/end/abort the "Just" fileds must be cleared and processed in the right way
  /// \param theTotalUpdate force to updates everything that has been changed in this operation
  void processOperation(const bool theTotalUpdate, const bool theFinish = false);

  /// Performs the feature execution
  /// \returns the status of execution
  void executeFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// returns true if the object was created or updated
  bool isUpdated(const std::shared_ptr<ModelAPI_Object>& theObj);
};

#endif
