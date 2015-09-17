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
  /// updated features during this transaction with IDs of iterations of modifications
  /// (to reexecute the object twice if needed: for correct preview, etc.)
  std::map<std::shared_ptr<ModelAPI_Object>, int > myUpdated;
  /// current id of modification inside of the current transaction
  int myModification;
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

  /// internal structure that contains the updating iteration information:
  /// which object and subobject is iterated, t ocontinue iteration
  class IterationItem {
    /// The main object, subs of it are iterated
    std::shared_ptr<ModelAPI_CompositeFeature> myMain;
    /// The currently iterated sub-object of root document
    std::shared_ptr<ModelAPI_Feature> mySub;
    /// If this is true, this iteration must be breaked immideately
    bool myBreaked;
    /// If this flag is true, the virtual iteration is performed, unbreackable
    bool myIsVirtual;
    /// For composite main contains the current index
    int myIndex;
    /// For root object constains the reference to objects
    Model_Objects* myObjects;
    /// If it is true, the next "next" will be ignored
    bool mySkipNext;

  public:
    /// Constructs the iterator of subs
    IterationItem(std::shared_ptr<ModelAPI_CompositeFeature> theFeature);
    /// Increments the iteration if not-breaked
    void next();
    /// Returns true if current is not null
    bool more();
    /// Returns the current iterated sub-element
    std::shared_ptr<ModelAPI_Feature> current();
    /// Returns true if theFeature is in all iterated objects 
    bool isIterated(std::shared_ptr<ModelAPI_Feature> theFeature);
    /// Returns true if theFeature is earlier than the current value 
    bool isEarlierThanCurrent(std::shared_ptr<ModelAPI_Feature> theFeature);
    /// Make iteration breaked
    void setBreaked();
    /// Makes the iteration before the currently breaked (next "next" will be ignored)
    void setCurrentBefore(std::shared_ptr<ModelAPI_Feature> theFeature);
    /// Initializes iteration: virtual or real
    void startIteration(const bool theVirtual);
    /// Returns true if iteration was breaked
    bool isBreaked() {return myBreaked;}
  };
  /// List of iterated features: composite feature to the currently iterated sub.
  /// The first element in the list has no "main": the root document is not feature.
  std::list<IterationItem> myProcessIterator;

 public:
  /// Is called only once, on startup of the application
  Model_Update();

  /// Processes the feature argument update: executes the results
  MODEL_EXPORT virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

protected:
  /// Recoursively checks and updates the feature if needed (calls the execute method)
  /// Returns true if feature was updated.
  void updateFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// Updates the selection and parametrical arguments before the later feature analysis
  /// Returns true if something really was updated
  void updateArguments(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// Sends the redisplay events for feature and results, updates the updated status
  void redisplayWithResults(std::shared_ptr<ModelAPI_Feature> theFeature, 
    const ModelAPI_ExecState theState);

  /// On operation start/end/abort the "Just" fileds must be cleared and processed in the right way
  /// \param theTotalUpdate force to updates everything that has been changed in this operation
  /// \param theFinish is true for start, close or abort transaction: all objects must be processed
  void processOperation(const bool theTotalUpdate, const bool theFinish = false);

  /// Performs the feature execution
  /// \returns the status of execution
  void executeFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// Iterates and updates features from theFeature by managing myProcessIterator.
  /// Returns only after the iteration is finished.
  /// \param theFeature is null for iteration of root document (which is not composite)
  /// \returns false if this feature should not be updated: iteration was moved much upper
  bool iterateUpdate(std::shared_ptr<ModelAPI_CompositeFeature> theFeature);
  /// Feature is updated during the execution, so in this case if feature was already executed,
  /// make the iterate Update process breaked to re-start iteration from this feature.
  void iterateUpdateBreak(std::shared_ptr<ModelAPI_Feature> theFeature);
  /// Returns true if the feature is older that the argument and it must be updated
  bool isOlder(std::shared_ptr<ModelAPI_Feature> theFeature, 
               std::shared_ptr<ModelAPI_Object> theArgument);
};

#endif
