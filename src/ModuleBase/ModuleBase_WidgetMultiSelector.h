// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetMultiSelector.h
 *
 *  Created on: Oct 8, 2014
 *      Author: sbh
 */

#ifndef MODULEBASE_WIDGETMULTISELECTOR_H_
#define MODULEBASE_WIDGETMULTISELECTOR_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

#include <GeomAPI_Shape.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_AttributeSelectionList.h>

#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>

#include <QList>
#include <QString>
#include <QStringList>
#include <QPair>

class QWidget;
class QListWidget;
class QComboBox;
class ModuleBase_IWorkshop;

class MODULEBASE_EXPORT ModuleBase_WidgetMultiSelector : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
  ModuleBase_WidgetMultiSelector(QWidget* theParent,
                                 ModuleBase_IWorkshop* theWorkshop,
                                 const Config_WidgetAPI* theData,
                                 const std::string& theParentId);
  virtual ~ModuleBase_WidgetMultiSelector();

  /// Saves the internal parameters to the given feature
  /// \param theObject a model feature to be changed
  virtual bool storeValue() const;

  virtual bool restoreValue();

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const;

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  virtual bool eventFilter(QObject* theObj, QEvent* theEvent);

 public slots:
  void activateSelection(bool toActivate);
  void onSelectionTypeChanged();
  void onSelectionChanged();

 protected:
  void filterShapes(const NCollection_List<TopoDS_Shape>& theShapesToFilter,
                    NCollection_List<TopoDS_Shape>& theResult);
  void setCurrentShapeType(const TopAbs_ShapeEnum theShapeType);
  void activateShapeSelection();

 private:
   void updateSelectionList(AttributeSelectionListPtr);

  QListWidget* myListControl;
  QComboBox* myTypeCombo;
  QWidget* myMainWidget;

  //TODO: Move into the base of selectors
  ModuleBase_IWorkshop* myWorkshop;

  /// If true then local selector has to be activated in context
  bool myUseSubShapes;
  bool myIsActive;

  typedef QPair<ResultPtr, GeomShapePtr> GeomSelection;
  QList<GeomSelection> mySelection;
};

#endif /* MODULEBASE_WIDGETFILESELECTOR_H_ */
