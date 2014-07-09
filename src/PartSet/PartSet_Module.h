#ifndef PartSet_Module_H
#define PartSet_Module_H

#include "PartSet.h"

#include <ModuleBase_IModule.h>
#include <ModuleBase_Definitions.h>
#include <XGUI_Command.h>
#include <ModelAPI_Feature.h>

#include <QMap>
#include <QObject>

#include <string>

#include <boost/shared_ptr.hpp>

class QMouseEvent;
class QKeyEvent;
class PartSet_Listener;
class ModelAPI_Feature;
class XGUI_ViewerPrs;
class ModuleBase_Operation;

class PARTSET_EXPORT PartSet_Module: public ModuleBase_IModule
{
Q_OBJECT

public:
  PartSet_Module(XGUI_Workshop* theWshop);
  virtual ~PartSet_Module();

  /// Returns the module workshop
  /// \returns a workshop instance
  XGUI_Workshop* workshop() const;

  /// Reads description of features from XML file 
  virtual void createFeatures();

  /// Called on creation of menu item in desktop
  virtual void featureCreated(QAction* theFeature);

  /// Returnc list of nested commands for the given feature
  virtual QStringList nestedFeatures(QString theFeature);
  std::string featureFile(const std::string&);

  /// Creates an operation and send it to loop
  /// \param theCmdId the operation name
  virtual void launchOperation(const QString& theCmdId);

  /// Called when it is necessary to update a command state (enable or disable it)
  //virtual bool isFeatureEnabled(const QString& theCmdId) const;

  /// Displays or erase the current operation preview, if it has it.
  /// \param theFeature the feature instance to be displayed
  /// \param isDisplay the state whether the presentation should be displayed or erased
  /// \param isUpdateViewer the flag whether the viewer should be updated
  void visualizePreview(FeaturePtr theFeature, bool isDisplay,
                        const bool isUpdateViewer = true);

  /// Activates the feature in the displayer
  /// \param theFeature the feature instance to be displayed
  /// \param isUpdateViewer the flag whether the viewer should be updated
  void activateFeature(FeaturePtr theFeature,
                       const bool isUpdateViewer);

  /// Updates current operation preview, if it has it.
  /// \param theCmdId the operation name
  void updateCurrentPreview(const std::string& theCmdId);

  /// Creates custom widgets for property panel
  virtual QWidget* createWidgetByType(const std::string& theType, QWidget* theParent, 
    Config_WidgetAPI* theWidgetApi, QList<ModuleBase_ModelWidget*>& theModelWidgets);


public slots:
  void onFeatureTriggered();
  /// SLOT, that is called after the operation is started. Connect on the focus activated signal
  void onOperationStarted();
  /// SLOT, that is called after the operation is stopped. Switched off the modfications performed
  /// by the operation start
  void onOperationStopped(ModuleBase_Operation* theOperation);
  /// SLOT, that is called afetr the popup menu action clicked.
  void onContextMenuCommand(const QString& theId, bool isChecked);
  /// SLOT, that is called by mouse press in the viewer.
  /// The mouse released point is sent to the current operation to be processed.
  /// \param theEvent the mouse event
  void onMousePressed(QMouseEvent* theEvent);
  /// SLOT, that is called by mouse release in the viewer.
  /// The mouse released point is sent to the current operation to be processed.
  /// \param theEvent the mouse event
  void onMouseReleased(QMouseEvent* theEvent);
  /// SLOT, that is called by mouse move in the viewer.
  /// The mouse moved point is sent to the current operation to be processed.
  /// \param theEvent the mouse event
  void onMouseMoved(QMouseEvent* theEvent);

  /// SLOT, that is called by the key in the viewer is clicked.
  /// \param theEvent the mouse event
  void onKeyRelease(QKeyEvent* theEvent);

  /// SLOT, that is called by the mouse double click in the viewer.
  /// \param theEvent the mouse event
  void onMouseDoubleClick(QMouseEvent* theEvent);

  /// SLOT, to apply to the current viewer the operation
  /// \param theX the X projection value
  /// \param theY the Y projection value
  /// \param theZ the Z projection value
  void onPlaneSelected(double theX, double theY, double theZ);

  /// SLOT, to fit all current viewer
  void onFitAllView();

  void onLaunchOperation(std::string theName, FeaturePtr theFeature);

  /// SLOT, to switch on/off the multi selection in the viewer
  /// \param theEnabled the enabled state
  void onMultiSelectionEnabled(bool theEnabled);

  /// SLOT, to stop or start selection mode for the features
  /// \param theFeatures a list of features to be disabled
  /// \param theToStop the boolean state whether it it stopped or non stopped
  void onStopSelection(const QFeatureList& theFeatures, const bool isStop);

  /// SLOT, to set selection
  /// \param theFeatures a list of features to be selected
  void onSetSelection(const QFeatureList& theFeatures);

  /// SLOT, to close the viewer local context
  void onCloseLocalContext();

  /// SLOT, to visualize the feature in another local context mode
  /// \param theFeature the feature to be put in another local context mode
  /// \param theMode the mode appeared on the feature
  void onFeatureConstructed(FeaturePtr theFeature,
                            int theMode);

  /// Slot which reacts to the point 2d set to the feature. Creates a constraint
  /// \param the feature
  /// \param the attribute of the feature
  void onStorePoint2D(FeaturePtr theFeature, const std::string& theAttribute);

protected:
  /// Creates a new operation
  /// \param theCmdId the operation name
  /// \param theFeatureKind a kind of feature to get the feature xml description
  ModuleBase_Operation* createOperation(const std::string& theCmdId,
                                        const std::string& theFeatureKind = "");

  /// Sends the operation
  /// \param theOperation the operation
  void sendOperation(ModuleBase_Operation* theOperation);

protected:
  //! Edits the feature
  void editFeature(FeaturePtr theFeature);

private:
  XGUI_Workshop* myWorkshop;
  PartSet_Listener* myListener;

  std::map<std::string, std::string> myFeaturesInFiles;
};

#endif
