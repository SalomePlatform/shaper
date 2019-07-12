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

#ifndef XGUI_ModuleConnector_H
#define XGUI_ModuleConnector_H

#include "XGUI.h"
#include <ModuleBase_Definitions.h>
#include <ModuleBase_IWorkshop.h>

class XGUI_Workshop;
class XGUI_Displayer;

/**
 * \ingroup GUI
 * Implementation of IWorkshop interface which provides access to Workshop services at module level
 */
class XGUI_EXPORT XGUI_ModuleConnector : public ModuleBase_IWorkshop
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theWorkshop a workshop
  XGUI_ModuleConnector(XGUI_Workshop* theWorkshop);

  virtual ~XGUI_ModuleConnector();

  //! Returns list of currently selected data objects
  virtual ModuleBase_ISelection* selection() const;

  //! Returns instance of loaded module
  virtual ModuleBase_IModule* module() const;

  //! Returns current viewer
  virtual ModuleBase_IViewer* viewer() const;

  //! Returns property panel
  virtual ModuleBase_IPropertyPanel* propertyPanel() const;

  //! Returns error manager
  virtual ModuleBase_IErrorMgr* errorMgr() const;

  /// A selection activate in 3D View handler
  virtual ModuleBase_ISelectionActivate* selectionActivate() const;

  //! Returns currently active operation
  virtual ModuleBase_Operation* currentOperation() const;

  //! Returns true if the operation with id theId can be started
  /// \param theId id of the operation which is going to start
  /// \param isCommitted boolean value if the operation was committed otherwise it was aborted
  virtual bool canStartOperation(QString theId, bool& isCommitted);

  //! Performs the operation launch
  //! \param theOperation an operation to be launched
  virtual void processLaunchOperation(ModuleBase_Operation* theOperation);

  //! Returns started operation by the operation identifier. The operation manager is called.
  //! \param theId an operation id
  //! \return an operation instance or NULL
  virtual ModuleBase_Operation* findStartedOperation(const QString& theId);

  //! Returns true if the operation with id theId can be stopped. The operation manager is called.
  //! \param theOperation a stopped operation
  //! \return boolean result
  virtual bool canStopOperation(ModuleBase_Operation* theOperation);

  //! Commits if possible or aborts the given operation.
  //! \param theOperation an aborted operation
  /// \param isCommitted boolean value if the operation was committed otherwise it was aborted
  virtual void stopOperation(ModuleBase_Operation* theOperation,
                             bool& isCommitted);

  //! Returns AIS object by data object
  virtual AISObjectPtr findPresentation(const ObjectPtr& theObject) const;

  //! Returns data object by AIS
  virtual ObjectPtr findPresentedObject(const AISObjectPtr& theAIS) const;

  //! Returns true if the object is displayed
  //! \param theObject a data object
  virtual bool isVisible(const ObjectPtr& theObject) const;

  //! Returns list of currently displayed objects
  virtual QObjectPtrList displayedObjects() const;

  //! Select features clearing previous selection.
  //! If the list is empty then selection will be cleared
  virtual void setSelected(const QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues);

  //! Shows the message in the status bar
  //! \param theMessage a message
  virtual void setStatusBarMessage(const QString& theMessage);

   /// Update of commands status
  virtual void updateCommandStatus();

  /// Returns Information about feature defined in corresponded XML
  /// \param theId - id of the feature
  virtual std::shared_ptr<Config_FeatureMessage> featureInfo(const QString& theId) const;

  /// Return application main window
  virtual QMainWindow* desktop() const;

  /// Returns true if SHIFT is pressed
  /// \return boolean value
  virtual bool hasSHIFTPressed() const;

  virtual void deactivateCurrentSelector();

  //! Temporary enable or disable viewer update. Returns previous state of updating
  //! \param isEnabled new state of the viewer update
  virtual bool enableUpdateViewer(bool isEnabled);

  //! Returns workshop
  XGUI_Workshop* workshop() const { return myWorkshop; }

private:
  QObjectPtrList activeObjects(const QObjectPtrList& theObjList) const;

  /// Reference to workshop
  XGUI_Workshop* myWorkshop;
};

#endif
