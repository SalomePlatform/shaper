// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef XGUI_WORKSHOP_LISTENER_H
#define XGUI_WORKSHOP_LISTENER_H

#include "XGUI.h"
#include <Events_Listener.h>
#include <Events_Message.h>

#include <ModelAPI_Feature.h>

#include <QObject>

class Config_FeatureMessage;

class ModelAPI_ObjectUpdatedMessage;
class XGUI_Workshop;

class ModuleBase_IWorkshop;

class QString;

/**\class XGUI_WorkshopListener
 * \ingroup GUI
 * \brief Class which process the events from the event loop.
 */
class XGUI_EXPORT XGUI_WorkshopListener : public QObject, public Events_Listener
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theConnector a Salome connector object. 
  /// Used only if the workshop is launched in Salome environment
   XGUI_WorkshopListener(ModuleBase_IWorkshop* theWorkshop);
  virtual ~XGUI_WorkshopListener();

  /// Register this class in the events loop for several types of events
  void initializeEventListening();

  //! Redefinition of Events_Listener method
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

signals:
  /// Emitted when error in applivation happens
  void errorOccurred(const QString&);

protected slots:
  /// Updates Apply All button state of the feature to the state if the feature has the button
  /// \param theFeatureId an index of the feature, the action is searched, which state is to be changed
  /// \param theState an action enable state
  //void onNestedStateChanged(const std::string& theFeatureId, const bool theState);

protected:
  /// Procedure to process postponed events
  bool event(QEvent * theEvent);

   /// Process event "Add a feature"
  void addFeature(const std::shared_ptr<Config_FeatureMessage>&);

  /// Process feature update message
  void onFeatureUpdatedMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& );

  /// Process feature created message
  void onFeatureCreatedMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& );

  /// Process feature redisplay message
  void onFeatureRedisplayMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& );

  /// Displaus object and fit all viewer if the object is first (update viewer will not be called)
  /// Asks the module whether the object can be displayed
  /// \param theObj an object
  /// \return true if the object is displayed
  bool displayObject(ObjectPtr theObj);

  /// Calls the module method of cusomize object for the feature of the current operation
  /// \return true if the object is modified
  bool customizeCurrentObject();

  /// Returns the workshop
  XGUI_Workshop* workshop() const;

private:
  ModuleBase_IWorkshop* myWorkshop; // the current workshop

  bool myUpdatePrefs;
};

#endif
