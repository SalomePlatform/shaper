// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
  /// Features which arguments were modified by not-persistent changes.
  /// So, these referencing arguments must be updated
  /// due to these features info also before execution).
  std::map<std::shared_ptr<ModelAPI_Feature>, std::set<std::shared_ptr<ModelAPI_Feature> > >
    myNotPersistentRefs;
  /// features that must be additionally processed after execution of finish operation
  std::set<std::shared_ptr<ModelAPI_Feature> > myWaitForFinish;
  /// to know that some parameter was changed during this operation (to enable update expressions)
  bool myIsParamUpdated;
  /// to execute features on finish if preview is needed only on finish operation
  bool myIsFinish;
  /// try if processing is currently performed
  bool myIsProcessed;
  /// map that contains features that must be executed only on finish of the operation
  /// the value in map is the set of reasons
  std::map<std::shared_ptr<ModelAPI_Feature>, std::set<std::shared_ptr<ModelAPI_Feature> > >
    myProcessOnFinish;
  /// to avoid infinitive cycling: feature -> count of the processing periods during this update
  std::map<std::shared_ptr<ModelAPI_Feature>, int > myProcessed;
  /// if preview in the property panel is blocked any update is postponed until end of operation
  bool myIsPreviewBlocked;
  /// disables any update if it is true, even on start/finish operation, undo, etc.
  bool myUpdateBlocked;

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
    const ModelAPI_ExecState theState, bool theUpdateState = true);

  /// On operation start/end/abort the "Just" fileds must be cleared and processed in the right way
  //! \param theFlushRedisplay a boolean value if the redisplay signal should be flushed
  void processFeatures(const bool theFlushRedisplay = true);

  /// Performs the feature execution
  /// \returns the status of execution
  void executeFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// Updates the properties of object because of stability state changes
  void updateStability(void* theSender);

  /// Returns true if theFeature modification was caused by theReason
  /// (may be feature of result of this feature)
  bool isReason(
    std::shared_ptr<ModelAPI_Feature>& theFeature, std::shared_ptr<ModelAPI_Object> theReason);

  /// Updates a selection attributes for the features that possible were affected by creation
  /// or reorder of features upper in the history line (issue #1757)
  void updateSelection(const std::set<std::shared_ptr<ModelAPI_Object> >& theObjects);

};

#endif
