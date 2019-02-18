// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef XGUI_WORKSHOP_LISTENER_H
#define XGUI_WORKSHOP_LISTENER_H

#include "XGUI.h"
#include <Events_Listener.h>
#include <Events_Message.h>

#include <ModelAPI_Feature.h>
#include <Events_InfoMessage.h>

#include <QObject>

class Config_FeatureMessage;

class ModelAPI_ObjectUpdatedMessage;
class XGUI_Workshop;

class ModuleBase_IWorkshop;

class QString;
class Events_InfoMessage;

/**\class XGUI_WorkshopListener
 * \ingroup GUI
 * \brief Class which process the events from the event loop.
 */
class XGUI_EXPORT XGUI_WorkshopListener : public QObject, public Events_Listener
{
  Q_OBJECT
public:
  /// Constructor. Used only if the workshop is launched in Salome environment
  /// \param theWorkshop a reference to workshop.
   XGUI_WorkshopListener(XGUI_Workshop* theWorkshop);
  virtual ~XGUI_WorkshopListener();

  /// Register this class in the events loop for several types of events
  void initializeEventListening();

  //! Redefinition of Events_Listener method
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

signals:
  /// Emitted when error in applivation happens
  void errorOccurred(std::shared_ptr<Events_InfoMessage> theMsg);

protected:
  /// Procedure to process postponed events
  bool event(QEvent * theEvent);

  /// Process feature update message
  /// \param theMsg a message with a container of objects
  void onFeatureUpdatedMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg);

  /// Process feature created message
  /// \param theMsg a message with a container of objects
  void onFeatureCreatedMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg);

  /// Process feature redisplay message
  /// \param theMsg a message with a container of objects
  void onFeatureRedisplayMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg);

  /// Process clearing of presentation
  /// \param theMsg an event message
  void onFeatureEmptyPresentationMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg);

  /// Displaus object and fit all viewer if the object is first (update viewer will not be called)
  /// Asks the module whether the object can be displayed
  /// \param theObj an object
  /// \param theFirstVisualizedBody an output state whether there are not object displayed in the
  /// view and the displayed object is a body
  /// \return true if the object is displayed
  bool displayObject(ObjectPtr theObj);

  /// Calls the module method of cusomize object for the feature of the current operation
  /// Perform cusomize if the force redisplay flag is true or the list of objects contains the
  /// current operation feature
  /// \param theObjects a list of objects to find current operation feature
  ///                   if forced redisplay is false
  /// \param theForceRedisplay a flag to customize object even always
  /// \return true if the object is modified
  bool customizeCurrentObject(const std::set<ObjectPtr>& theObjects, bool theForceRedisplay);

  /// Returns the workshop
  XGUI_Workshop* workshop() const;

private:
  XGUI_Workshop* myWorkshop; // the current workshop

  bool myUpdatePrefs;
};

#endif
