// File:        ModuleBase_WidgetShapeSelector.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_WidgetShapeSelector_H
#define ModuleBase_WidgetShapeSelector_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"
#include "ModuleBase_ViewerFilters.h"

#include <ModelAPI_Object.h>
#include <GeomAPI_Shape.h>

#include <TopAbs_ShapeEnum.hxx>

#include <QStringList>

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
  static TopAbs_ShapeEnum shapeType(const QString& theType);

  ModuleBase_WidgetShapeSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                            const Config_WidgetAPI* theData, const std::string& theParentId);

  virtual ~ModuleBase_WidgetShapeSelector();

  /// Saves the internal parameters to the given feature
  virtual bool storeValue() const;

  virtual bool restoreValue();

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

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValue the wrapped widget value
  virtual bool setValue(ModuleBase_WidgetValue* theValue);


  /// The methiod called when widget is activated
  virtual void activate();

  /// The methiod called when widget is deactivated
  virtual void deactivate();


 public slots:

  /// Activate or deactivate selection
  void activateSelection(bool toActivate);

 private slots:
  void onSelectionChanged();

 protected:
  void updateSelectionName();
  void raisePanel() const;

  /// Returns true if shape of given object corresponds to requested shape type
  /// This method is called only in non sub-shapes selection mode
  virtual bool acceptObjectShape(const ObjectPtr theObject) const;

  /// Returns true if selected shape corresponds to requested shape types
  /// This method is called only in sub-shapes selection mode
  virtual bool acceptSubShape(std::shared_ptr<GeomAPI_Shape> theShape) const;

  /// Returns true if selected object corresponds to requested Object type
  /// Thid method is used in any selection mode
  virtual bool acceptObjectType(const ObjectPtr theObject) const;


  // Set the given object as a value of the widget
  void setObject(ObjectPtr theObj, std::shared_ptr<GeomAPI_Shape> theShape = std::shared_ptr<GeomAPI_Shape>());

  /// Check the selected with validators if installed
  virtual bool isValid(ObjectPtr theObj, std::shared_ptr<GeomAPI_Shape> theShape);

  /// Clear attribute
  void clearAttribute();

  //----------- Class members -------------
 protected:
  QWidget* myContainer;
  QLabel* myLabel;
  QLineEdit* myTextLine;

  ModuleBase_IWorkshop* myWorkshop;

  ObjectPtr mySelectedObject;
  std::shared_ptr<GeomAPI_Shape> myShape;

  QStringList myShapeTypes;
  QStringList myObjectTypes;

  /// If true then local selector has to be activated in context
  bool myUseSubShapes;

  bool myIsActive;

  Handle(ModuleBase_ObjectTypesFilter) myObjTypeFilter;
};

#endif
