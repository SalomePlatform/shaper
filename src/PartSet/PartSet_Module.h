#ifndef PartSet_Module_H
#define PartSet_Module_H

#include "PartSet.h"

#include <XGUI_Module.h>
#include <XGUI_Command.h>

#include <QMap>
#include <QObject>

#include <string>

#include <boost/shared_ptr.hpp>

class QMouseEvent;
class QKeyEvent;
class PartSet_Listener;
class ModelAPI_Feature;
class XGUI_ViewerPrs;

class PARTSET_EXPORT PartSet_Module: public QObject, public XGUI_Module
{
Q_OBJECT

public:
  PartSet_Module(XGUI_Workshop* theWshop);
  virtual ~PartSet_Module();

  /// Returns the module workshop
  /// \returns a workshop instance
  XGUI_Workshop* workshop() const;

  virtual void createFeatures();
  virtual void featureCreated(QAction* theFeature);
  virtual QStringList nestedFeatures(QString theFeature);
  std::string featureFile(const std::string&);

  /// Creates an operation and send it to loop
  /// \param theCmdId the operation name
  virtual void launchOperation(const QString& theCmdId);

  /// Displays or erase the current operation preview, if it has it.
  /// \param theFeature the feature instance to be displayed
  /// \param isDisplay the state whether the presentation should be displayed or erased
  /// \param isUpdateViewer the flag whether the viewer should be updated
  void visualizePreview(boost::shared_ptr<ModelAPI_Feature> theFeature, bool isDisplay,
                        const bool isUpdateViewer = true);

  /// Activates the feature in the displayer
  /// \param theFeature the feature instance to be displayed
  /// \param isUpdateViewer the flag whether the viewer should be updated
  void activateFeature(boost::shared_ptr<ModelAPI_Feature> theFeature,
                       const bool isUpdateViewer);

  /// Updates current operation preview, if it has it.
  /// \param theCmdId the operation name
  void updateCurrentPreview(const std::string& theCmdId);

public slots:
  void onFeatureTriggered();
  /// SLOT, that is called after the operation is stopped. Switched off the modfications performed
  /// by the operation start
  void onOperationStopped(ModuleBase_Operation* theOperation);

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
  void onKeyRelease(QKeyEvent*);

  /// SLOT, to apply to the current viewer the operation
  /// \param theX the X projection value
  /// \param theY the Y projection value
  /// \param theZ the Z projection value
  void onPlaneSelected(double theX, double theY, double theZ);

  void onLaunchOperation(std::string theName, boost::shared_ptr<ModelAPI_Feature> theFeature);

  /// SLOT, to switch on/off the multi selection in the viewer
  /// \param theEnabled the enabled state
  void onMultiSelectionEnabled(bool theEnabled);

  /// SLOT, to stop or start selection mode for the features
  /// \param theFeatures a list of features to be disabled
  /// \param theToStop the boolean state whether it it stopped or non stopped
  void onStopSelection(const std::list<XGUI_ViewerPrs>& theFeatures, const bool isStop);

  /// SLOT, to set selection
  /// \param theFeatures a list of features to be selected
  void onSetSelection(const std::list<XGUI_ViewerPrs>& theFeatures);

  /// SLOT, to close the viewer local context
  void onCloseLocalContext();

  /// SLOT, to visualize the feature in another local context mode
  /// \param theFeature the feature to be put in another local context mode
  /// \param theMode the mode appeared on the feature
  void onFeatureConstructed(boost::shared_ptr<ModelAPI_Feature> theFeature,
                            int theMode);
protected:
  /// Creates a new operation
  /// \param theCmdId the operation name
  ModuleBase_Operation* createOperation(const std::string& theCmdId);

  /// Sends the operation
  /// \param theOperation the operation
  void sendOperation(ModuleBase_Operation* theOperation);

private:
  XGUI_Workshop* myWorkshop;
  PartSet_Listener* myListener;

  std::map<std::string, std::string> myFeaturesInFiles;
};

#endif
