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
  /// updated features during this transaction: must be updated immediately
  std::set<std::shared_ptr<ModelAPI_Object> > myJustUpdated;
  /// features that must be additionally processed after execution of finish operation
  std::set<std::shared_ptr<ModelAPI_Object> > myWaitForFinish;
  /// to know that all next updates are caused by this execution
  bool myIsExecuted;
  /// to know execute or not automatically all update
  bool myIsAutomatic;
  /// to know that some parameter was changed during this operation
  bool myIsParamUpdated;
  /// to execute features of finish if perview is not needed
  bool myIsFinish;

 public:
  /// Is called only once, on startup of the application
  Model_Update();

  /// Processes the feature argument update: executes the results
  MODEL_EXPORT virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

protected:
  /// Recoursively checks and updates the feature if needed (calls the execute method)
  /// Returns true if feature was updated.
  void updateFeature(std::shared_ptr<ModelAPI_Feature> theFeature,
    std::set<std::shared_ptr<ModelAPI_Feature> >& theProcessed);

  /// Updates the selection and parametrical arguments before the later feature analysis
  /// Returns true if something really was updated
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
};

#endif
