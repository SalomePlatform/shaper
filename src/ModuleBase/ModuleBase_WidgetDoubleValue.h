// File:        ModuleBase_WidgetDoubleValue.h
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_WidgetDoubleValue_H
#define ModuleBase_WidgetDoubleValue_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class ModuleBase_DoubleSpinBox;
class Config_WidgetAPI;
class QWidget;
class QLabel;
class QTimer;

class MODULEBASE_EXPORT ModuleBase_WidgetDoubleValue : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \theParent the parent object
  /// \theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetDoubleValue(QWidget* theParent, const Config_WidgetAPI* theData,
                               const std::string& theParentId);

  virtual ~ModuleBase_WidgetDoubleValue();

  /// Saves the internal parameters to the given feature
  /// \param theObject a model feature to be changed
  virtual bool storeValue() const;

  virtual bool restoreValue();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const
  {
    return myContainer;
  }

  /// Process key release envent on the widget spin box controls
  /// \param theObject the object where the event happens
  /// \param theEvent the processed event
  virtual bool eventFilter(QObject *theObject, QEvent *theEvent);

 public slots:
 /// Delayed value chnged: when user starts typing something,
 // it gives him a 0,5 second to finish typing, when sends valueChnaged() signal
//  void onValueChanged();

 protected:
  QWidget* myContainer;
  QLabel* myLabel;
  ModuleBase_DoubleSpinBox* mySpinBox;
};

#endif
