// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef ModuleBase_WidgetSelectionFilter_H
#define ModuleBase_WidgetSelectionFilter_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"
#include "ModuleBase_ViewerPrs.h"

#include <ModelAPI_Filter.h>

#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <AIS_Shape.hxx>
#include <AIS_ListOfInteractive.hxx>

#include <QWidget>
#include <QList>

class QLabel;
class QComboBox;
class QGroupBox;
class QToolButton;
class QVBoxLayout;
class QPushButton;
class QCheckBox;

class ModuleBase_IWorkshop;

class MODULEBASE_EXPORT ModuleBase_FilterStarter: public QWidget
{
  Q_OBJECT
public:
  ModuleBase_FilterStarter(const std::string& theFeature, QWidget* theParent,
    ModuleBase_IWorkshop* theWorkshop);

  ~ModuleBase_FilterStarter() {}

private slots:
  void onFiltersLaunch();

private:
  std::string myFeatureName;
  ModuleBase_IWorkshop* myWorkshop;

  QLabel* myFilterLbl;
  QLabel* myModifyLbl;
};


class ModuleBase_WidgetSelectionFilter;

class ModuleBase_FilterItem : public QWidget
{
  Q_OBJECT
public:
  ModuleBase_FilterItem(const std::string& theFilter, ModuleBase_WidgetSelectionFilter* theParent);

  std::string filter() const { return myFilterID; }

  /// Returns list of widget controls
  /// \return a control list
  QList<QWidget*> getControls() const;

  QList<ModuleBase_ModelWidget*> widgets() const {
    return myWidgets;
  }

signals:
  void deleteItem(ModuleBase_FilterItem* theItem);
  void reversedItem(ModuleBase_FilterItem* theItem);

private slots:
  void onReverse(bool theCheck);
  void onDelete();

private:
  void addItemRow(QWidget* theParent);

  std::string myFilterID;
  FiltersFeaturePtr mySelection;
  QToolButton* myRevBtn;
  QList<ModuleBase_ModelWidget*> myWidgets;
};

class ModuleBase_WidgetSelectionFilter : public ModuleBase_ModelWidget
{
  Q_OBJECT
public:
  ModuleBase_WidgetSelectionFilter(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
    const Config_WidgetAPI* theData);

  ~ModuleBase_WidgetSelectionFilter();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// It is called when user press Ok or OkPlus buttons in the parent property panel
  /// By default this slot does nothing
  virtual void onFeatureAccepted();

  ModuleBase_IWorkshop* workshop() const { return myWorkshop; }

  QWidget* filtersWidget() const { return myFiltersWgt; }

protected:
  /// Saves the internal parameters to the given feature (not ussed for this widget)
  /// \return True in success
  virtual bool storeValueCustom();

  /// Restore value from attribute data to the widget's control (not ussed for this widget)
  virtual bool restoreValueCustom();

private slots:
  void onAddFilter(int);
  void onDeleteItem(ModuleBase_FilterItem* theItem);
  void onReverseItem(ModuleBase_FilterItem* theItem);
  void onSelect();
  void onShowOnly(bool theErase);

private:
  void updateSelectBtn();
  void updateNumberSelected();
  void clearCurrentSelection(bool toUpdate = false);
  void updatePreview(const TopoDS_Shape& theShape);
  void redisplayFeature();

private:
  ModuleBase_IWorkshop* myWorkshop;

  QWidget* myFiltersWgt;
  QVBoxLayout* myFiltersLayout;

  QComboBox* myFiltersCombo;
  QPushButton* mySelectBtn;
  QLabel* myNbLbl;
  QCheckBox* myShowBtn;

  int mySelectionType;
  std::list<std::string> myFilters;
  std::list<std::string> myUseFilters;

  QList<ModuleBase_ViewerPrsPtr> myValues;
  Handle(AIS_Shape) myPreview;

  AIS_ListOfInteractive myListIO;

  FeaturePtr mySelectorFeature;
  std::string mySelectorAttribute;
};


#endif