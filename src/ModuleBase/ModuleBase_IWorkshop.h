// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef ModuleBase_IWorkshop_H
#define ModuleBase_IWorkshop_H

#include "ModuleBase.h"
#include "ModuleBase_Definitions.h"
#include <ModuleBase_FilterValidated.h>

#include <ModelAPI_Object.h>
#include <GeomAPI_AISObject.h>
#include <Config_FeatureMessage.h>

#include <QObject>

class ModuleBase_IModule;
class ModuleBase_ISelection;
class ModuleBase_IViewer;
class ModuleBase_IPropertyPanel;
class ModuleBase_IErrorMgr;
class ModuleBase_Operation;
class ModuleBase_ISelectionActivate;
class ModuleBase_ViewerPrs;
class QMainWindow;

/**
 * \ingroup GUI
 * Class which provides access to Workshop object services
 */
class MODULEBASE_EXPORT ModuleBase_IWorkshop : public QObject
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theParent parent object
  ModuleBase_IWorkshop(QObject* theParent);

  virtual ~ModuleBase_IWorkshop()
  {}

  /// Return current selection instance
  virtual ModuleBase_ISelection* selection() const = 0;

  //! Returns instance of loaded module
  virtual ModuleBase_IModule* module() const = 0;

  //! Returns current viewer
  virtual ModuleBase_IViewer* viewer() const = 0;

  //! Returns property panel
  virtual ModuleBase_IPropertyPanel* propertyPanel() const = 0;

  //! Returns error manager
  virtual ModuleBase_IErrorMgr* errorMgr() const = 0;

  /// A filter to process an attribute validators
  /// \return a filter
  Handle(ModuleBase_FilterValidated) validatorFilter();

  /// A selection activate in 3D View handler
  virtual ModuleBase_ISelectionActivate* selectionActivate() const = 0;

  //! Returns currently active operation
  virtual ModuleBase_Operation* currentOperation() const = 0;

  //! Returns true if the operation with id theId can be started
  virtual bool canStartOperation(QString theId, bool& isCommitted) = 0;

  //! Performs the operation launch
  //! \param theOperation an operation to be launched
  virtual void processLaunchOperation(ModuleBase_Operation* theOperation) = 0;

  //! Returns started operation by the operation identifier
  //! \param theId an operation id
  //! \return an operation instance or NULL
  virtual ModuleBase_Operation* findStartedOperation(const QString& theId) = 0;

  //! Returns true if the operation with id theId can be stopped
  //! \param theOperation a stopped operation
  //! \return boolean result
  virtual bool canStopOperation(ModuleBase_Operation* theOperation) = 0;

  //! Commits if possible or aborts the given operation.
  //! \param theOperation an aborted operation
  /// \param isCommitted boolean value if the operation was committed otherwise it was aborted
  virtual void stopOperation(ModuleBase_Operation* theOperation,
                             bool& isCommitted) = 0;

  //! Returns AIS object by data object
  //! \param theObject a data object
  virtual AISObjectPtr findPresentation(const ObjectPtr& theObject) const = 0;

  //! Returns data object by AIS
  //! \param theAIS a presentation
  virtual ObjectPtr findPresentedObject(const AISObjectPtr& theAIS) const = 0;

  //! Returns true if the object is displayed
  //! \param theObject a data object
  virtual bool isVisible(const ObjectPtr& theObject) const = 0;

  //! Select features clearing previous selection.
  //! If the list is empty then selection will be cleared
  //! \param theValues a list of presentations
  virtual void setSelected(const QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues) = 0;

  //! Shows the message in the status bar
  //! \param theMessage a message
  virtual void setStatusBarMessage(const QString& theMessage) = 0;

   /// Update of commands status
  virtual void updateCommandStatus() = 0;

  /// Returns feature info according to given feature ID
  /// \param theId a feature Id
  virtual std::shared_ptr<Config_FeatureMessage> featureInfo(const QString& theId) const = 0;

  /// Returns main window of the application
  virtual QMainWindow* desktop() const = 0;

  /// Returns true if SHIFT is pressed
  /// \return boolean value
  virtual bool hasSHIFTPressed() const = 0;

signals:
  /// Signal selection changed.
  void selectionChanged();

  /// Signal which is emited after activation of property panel
  void propertyPanelActivated();

protected:
  /// A filter to process an attribute validators
  Handle(ModuleBase_FilterValidated) myValidatorFilter;
};

#endif
