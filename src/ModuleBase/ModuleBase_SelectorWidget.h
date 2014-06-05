// File:        ModuleBase_SelectorWidget.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_SelectorWidget_H
#define ModuleBase_SelectorWidget_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

#include <ModelAPI_Feature.h>


class Config_WidgetAPI;
class QWidget;
class QLabel;
class QLineEdit;
class QToolButton;
class ModuleBase_IWorkshop;

class MODULEBASE_EXPORT ModuleBase_SelectorWidget: public ModuleBase_ModelWidget
{
  Q_OBJECT
public:
  ModuleBase_SelectorWidget(QWidget* theParent, 
                            ModuleBase_IWorkshop* theWorkshop, 
                            const Config_WidgetAPI* theData);

  virtual ~ModuleBase_SelectorWidget();

  /// Saves the internal parameters to the given feature
  /// \param theFeature a model feature to be changed
  virtual bool storeValue(FeaturePtr theFeature) const;

  virtual bool restoreValue(FeaturePtr theFeature);

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const { return myContainer; }

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  void setActivationOnStart(bool toActivate) { myActivateOnStart = toActivate; }
  bool activateOnStart() const { return myActivateOnStart; }

  FeaturePtr selectedFeature() const { return mySelectedFeature; }

public slots:

  /// Activate or deactivate selection
  void activateSelection(bool toActivate);

protected:
  bool eventFilter(QObject* theObj, QEvent* theEvent);

private slots:
  void onSelectionChanged();

private:
  void enableOthersControls(bool toEnable) const;
  void updateSelectionName();
  void raisePanel() const;

  std::string myFeatureAttributeID;

  QWidget*     myContainer;
  QLabel*      myLabel;
  QLineEdit*   myTextLine;
  QToolButton* myActivateBtn;

  ModuleBase_IWorkshop* myWorkshop;

  bool myActivateOnStart;

  FeaturePtr mySelectedFeature;
};

#endif