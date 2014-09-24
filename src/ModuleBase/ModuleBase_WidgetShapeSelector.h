// File:        ModuleBase_WidgetShapeSelector.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_WidgetShapeSelector_H
#define ModuleBase_WidgetShapeSelector_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

#include <ModelAPI_Object.h>

#include <TopAbs_ShapeEnum.hxx>

#include <QStringList>
#include <QPalette>

class Config_WidgetAPI;
class QWidget;
class QLabel;
class QLineEdit;
class QToolButton;
class ModuleBase_IWorkshop;

class MODULEBASE_EXPORT ModuleBase_WidgetShapeSelector : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  ModuleBase_WidgetShapeSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                            const Config_WidgetAPI* theData, const std::string& theParentId);

  virtual ~ModuleBase_WidgetShapeSelector();

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

  ObjectPtr selectedFeature() const
  {
    return mySelectedObject;
  }

 public slots:

  /// Activate or deactivate selection
  void activateSelection(bool toActivate);

private slots:
  void onSelectionChanged();

protected:
  bool eventFilter(QObject* theObj, QEvent* theEvent);

private:
  void updateSelectionName();
  void raisePanel() const;
  bool isAccepted(const ObjectPtr theObject) const;

  static TopAbs_ShapeEnum shapeType(const QString& theType);

  QWidget* myContainer;
  QLabel* myLabel;
  QLineEdit* myTextLine;
  //QToolButton* myActivateBtn;

  ModuleBase_IWorkshop* myWorkshop;

  ObjectPtr mySelectedObject;
  QStringList myShapeTypes;

  QPalette myBasePalet;
  QPalette myInactivePalet;

  bool myIsActive;
};

#endif
