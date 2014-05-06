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
  virtual void featureCreated(XGUI_Command* theFeature);
  virtual QStringList nestedFeatures(QString theFeature);
  std::string featureFile(const std::string&);

  /// Creates an operation and send it to loop
  /// \param theCmdId the operation name
  virtual void launchOperation(const QString& theCmdId);

  /// Displays or erase the current operation preview, if it has it.
  /// \param theF
  /// \param isDisplay the state whether the presentation should be displayed or erased
  void visualizePreview(boost::shared_ptr<ModelAPI_Feature> theFeature, bool isDisplay);

public slots:
  void onFeatureTriggered();
  /// SLOT, that is called after the operation is stopped. Disconnect the sketch feature
  /// from the viewer selection and show the sketch preview.
  void onOperationStopped(ModuleBase_Operation* theOperation);

  /// SLOT, that is called by the selection in the viewer is changed.
  /// The selection is sent to the current operation if it listens selection.
  void onSelectionChanged();
  /// SLOT, that is called by mouse click in the viewer.
  /// The mouse released point is sent to the current operation to be processed.
  /// \param theEvent the mouse event
  void onMouseReleased(QMouseEvent* theEvent);
  /// SLOT, that is called by the selection in the viewer is changed.
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

  /// SLOT, to visualize the feature in another local context mode
  /// \param theFeature the feature to be put in another local context mode
  /// \param theMode the mode appeared on the feature
  void onFeatureConstructed(boost::shared_ptr<ModelAPI_Feature> theFeature,
                            int theMode);
protected:
  /// Creates a new operation
  /// \param theCmdId the operation name
  ModuleBase_Operation* createOperation(const QString& theCmdId);

  /// Sends the operation
  /// \param theOperation the operation
  void sendOperation(ModuleBase_Operation* theOperation);

private:
  XGUI_Workshop* myWorkshop;
  PartSet_Listener* myListener;

  std::map<std::string, std::string> myFeaturesInFiles;
};

#endif
