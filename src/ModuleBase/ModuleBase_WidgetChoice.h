// File:        ModuleBase_WidgetChoice.h
// Created:     03 Sept 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_WidgetChoice_H
#define ModuleBase_WidgetChoice_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class QWidget;
class QLabel;
class QComboBox;

class MODULEBASE_EXPORT ModuleBase_WidgetChoice : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  ModuleBase_WidgetChoice(QWidget* theParent, const Config_WidgetAPI* theData, 
                          const std::string& theParentId);

  virtual ~ModuleBase_WidgetChoice();
  
  /// Saves the internal parameters to the given feature
  /// \param theObject a model feature to be changed
  virtual bool storeValue() const;

  virtual bool restoreValue();

  virtual bool focusTo();

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const
  {
    return myContainer;
  }

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

private slots:
  void onCurrentIndexChanged(int theIndex);

private:
  QWidget* myContainer;
  QLabel* myLabel;
  QComboBox* myCombo;
};

#endif
