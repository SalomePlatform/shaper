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
  virtual bool storeValue(FeaturePtr theFeature);

  virtual bool restoreValue(FeaturePtr theFeature);

  /// Returns whether the widget can accept focus, or if it corresponds to the given attribute
  /// \param theAttribute name
  virtual bool canFocusTo(const std::string& theAttributeName);

  /// Set focus to the current widget if it corresponds to the given attribute
  virtual void focusTo();

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const { return myContainer; }

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

private slots:
  void onSelectionChanged();

private:
  std::string myFeatureAttributeID;

  QWidget*     myContainer;
  QLabel*      myLabel;
  QLineEdit*   myTextLine;
  QToolButton* myActivateBtn;

  ModuleBase_IWorkshop* myWorkshop;
};

#endif