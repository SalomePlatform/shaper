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
class ModelAPI_CompositeFeature;
class Model_Objects;

/**\class Model_Update
 * \ingroup DataModel
 * \brief Updates the results of features when it is needed.
 */
class Model_Update : public Events_Listener
{
  /// Features and results that were modified and not yet processed.
  /// The second set is the objects that causes this object is modified
  std::map<std::shared_ptr<ModelAPI_Feature>, std::set<std::shared_ptr<ModelAPI_Feature> > >
    myModified;
  /// features that must be additionally processed after execution of finish operation
  std::set<std::shared_ptr<ModelAPI_Feature> > myWaitForFinish;
  /// to know that some parameter was changed during this operation (to enable update expressions)
  bool myIsParamUpdated;
  /// to execute features on finish if preview is needed only on finish operation
  bool myIsFinish;
  /// try if processing is currently performed
  bool myIsProcessed;
  /// set that contains features that must be executed only on finish of the operation
  std::set<std::shared_ptr<ModelAPI_Feature> > myProcessOnFinish;
  /// to avoid infinitive cycling: feature -> count of the processing periods during this update
  std::map<std::shared_ptr<ModelAPI_Feature>, int > myProcessed;
  /// if preview in hte property panel is blocked and any update is postponed until the end of operation
  bool myIsPreviewBlocked;

 public:
  /// Is called only once, on startup of the application
  Model_Update();

  /// Processes the feature argument update: executes the results
  MODEL_EXPORT virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

protected:
  /// Appends the new modified feature to the myModified, clears myProcessed if needed
  /// Returns true if some feature really was marked as modified
  /// theReason is the object that causes modification of this feature
  /// returns true if something reallsy was added to the modified and must be processed
  bool addModified(
    std::shared_ptr<ModelAPI_Feature> theFeature, std::shared_ptr<ModelAPI_Feature> theReason);

  /// Recoursively checks and updates features if needed (calls the execute method)
  /// Returns true if feature was updated.
  bool processFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// Updates the selection and parametrical arguments before the later feature analysis
  /// Returns true if something really was updated
  void updateArguments(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// Sends the redisplay events for feature and results, updates the updated status
  void redisplayWithResults(std::shared_ptr<ModelAPI_Feature> theFeature, 
    const ModelAPI_ExecState theState);

  /// On operation start/end/abort the "Just" fileds must be cleared and processed in the right way
  /// \param theFinish is true for start, close or abort transaction: all objects must be processed
  void processFeatures();

  /// Performs the feature execution
  /// \returns the status of execution
  void executeFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// Updates the properties of object because of stability state changes
  void updateStability(void* theSender);

  /// Returns true if theFeature modification was caused by theReason (may be feature of result of this feature)
  bool isReason(
    std::shared_ptr<ModelAPI_Feature>& theFeature, std::shared_ptr<ModelAPI_Object> theReason);
};

#endif
