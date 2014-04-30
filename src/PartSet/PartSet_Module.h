#ifndef PartSet_Module_H
#define PartSet_Module_H

#include "PartSet.h"

#include <XGUI_Module.h>
#include <XGUI_Command.h>

#include <QMap>
#include <QObject>

#include <string>

#include <boost/shared_ptr.hpp>

class XGUI_ViewWindow;
class QMouseEvent;
class PartSet_Listener;
class ModelAPI_Feature;

class PARTSET_EXPORT PartSet_Module: public QObject, public XGUI_Module
{
Q_OBJECT

public:
  PartSet_Module(XGUI_Workshop* theWshop);
  virtual ~PartSet_Module();

  virtual void createFeatures();
  virtual void featureCreated(XGUI_Command* theFeature);
  virtual QStringList nestedFeatures(QString theFeature);
  std::string featureFile(const std::string&);

  virtual void launchOperation(const QString& theCmdId);

  /// Displays or erase the current operation preview, if it has it.
  /// \param theF
  /// \param isDisplay the state whether the presentation should be displayed or erased
  void visualizePreview(boost::shared_ptr<ModelAPI_Feature> theFeature, bool isDisplay);

public slots:
  void onFeatureTriggered();
  /// SLOT, that is called after the operation is started. Perform some specific for module
  /// actions, e.g. connect the sketch feature to the viewer selection and show the sketch preview.
  void onOperationStarted();
  /// SLOT, that is called after the operation is stopped. Disconnect the sketch feature
  /// from the viewer selection and show the sketch preview.
  void onOperationStopped(ModuleBase_Operation* theOperation);

  /// SLOT, that is called by the selection in the viewer is changed.
  /// The selection is sent to the current operation if it listens selection.
  void onSelectionChanged();
  /// SLOT, that is called by mouse click in the viewer.
  /// The mouse released point is sent to the current operation to be processed.
  /// \param theWindow the window where the signal appears
  /// \param theEvent the mouse event
  void onMouseReleased(XGUI_ViewWindow* theWindow, QMouseEvent* theEvent);
  /// SLOT, that is called by the selection in the viewer is changed.
  /// The mouse moved point is sent to the current operation to be processed.
  /// \param theWindow the window where the signal appears
  /// \param theEvent the mouse event
  void onMouseMoved(XGUI_ViewWindow* theWindow, QMouseEvent* theEvent);

  /// SLOT, to apply to the current viewer the operation
  /// \param theX the X projection value
  /// \param theY the Y projection value
  /// \param theZ the Z projection value
  void onPlaneSelected(double theX, double theY, double theZ);

  /// SLOT, to visualize the feature in another local context mode
  /// \param theFeature the feature to be put in another local context mode
  void onFeatureConstructed(boost::shared_ptr<ModelAPI_Feature> theFeature);

private:
  XGUI_Workshop* myWorkshop;
  PartSet_Listener* myListener;

  std::map<std::string, std::string> myFeaturesInFiles;
};

#endif
