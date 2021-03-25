// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <ModelAPI_FiltersFeature.h>

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

class ModelAPI_Filter;

class ModuleBase_IWorkshop;

/**
* \ingroup GUI
* An object which lets to start a Filters operation as
* a sub-operation of the current one.
*/
class MODULEBASE_EXPORT ModuleBase_FilterStarter: public QWidget
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theFeature a name of feature for filtering
  /// \param theParent a parent widget which will get control after filtering
  /// \param theWorkshop a pointer on a current workshop
  ModuleBase_FilterStarter(const std::string& theFeature, QWidget* theParent,
    ModuleBase_IWorkshop* theWorkshop);

  /// Destructor
  ~ModuleBase_FilterStarter() {}

private slots:
  /// A slot to launch filtering operation
  void onFiltersLaunch();

private:
  /// Name of filtering feature
  std::string myFeatureName;

  /// A workshop
  ModuleBase_IWorkshop* myWorkshop;
};


class ModuleBase_WidgetSelectionFilter;
/**
* \ingroup GUI
* A widget which reperesents a one filter item in filters list
* Also it includes filter GUI if it exists
*/
class ModuleBase_FilterItem : public QWidget
{
  Q_OBJECT
public:

  /// Constructor
  /// \param theFilter the filter ID
  /// \param theParent a parent widget of ModuleBase_WidgetSelectionFilter class
  ModuleBase_FilterItem(const std::string& theFilter, ModuleBase_WidgetSelectionFilter* theParent);

  /// Returns filter Id
  std::string filter() const { return myFilterID; }

  /// Returns list of widgets which reperesent the current filter GUI
  QList<ModuleBase_ModelWidget*> widgets() const {
    return myWidgets;
  }

signals:
  /// The seignal is sent on deletion of the item
  void deleteItem(ModuleBase_FilterItem* theItem);

  /// The seignal is sent on reversing of the item
  void reversedItem(ModuleBase_FilterItem* theItem);

private slots:
  /// A slot to process reverse button click
  /// \param theCheck a current state of the button
  void onReverse(bool theCheck);

  /// A slot to process delete button click
  void onDelete();

private:
  /// A function which adds standard widgets of the item
  void addItemRow(QWidget* theParent);

  /// Current filter Id
  std::string myFilterID;

  /// Filters feature
  FiltersFeaturePtr mySelection;

  /// Reverce button
  QToolButton* myRevBtn;

  /// A list of sub-widgets
  QList<ModuleBase_ModelWidget*> myWidgets;
};

/**
* \ingroup GUI
* A widget for selection by filters
*/
class ModuleBase_WidgetSelectionFilter : public ModuleBase_ModelWidget
{
  Q_OBJECT
public:
  static FeaturePtr SelectorFeature;
  static std::string AttributeId;


  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  /// a low-level API for reading xml definitions of widgets
  ModuleBase_WidgetSelectionFilter(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
    const Config_WidgetAPI* theData, bool theReadOnly = false);

  /// Destructor
  ~ModuleBase_WidgetSelectionFilter();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// It is called when user press Ok or OkPlus buttons in the parent property panel
  /// By default this slot does nothing
  virtual void onFeatureAccepted();

  /// Returns current workshop
  ModuleBase_IWorkshop* workshop() const { return myWorkshop; }

  /// Returns a container widget for filter items
  QWidget* filtersWidget() const { return myFiltersWgt; }

  /// Returns error string
  virtual QString getError(const bool theValueStateChecked = true) const;

protected:
  /// Saves the internal parameters to the given feature (not ussed for this widget)
  /// \return True in success
  virtual bool storeValueCustom();

  /// Restore value from attribute data to the widget's control (not ussed for this widget)
  virtual bool restoreValueCustom();

private slots:
  /// Add a filter by Id in combo box
  void onAddFilter(int);

  /// Process deletion of a filter item
  void onDeleteItem(ModuleBase_FilterItem* theItem);

  /// Process reversion of a filter item
  void onReverseItem(ModuleBase_FilterItem* theItem);

  /// Process selection
  void onSelect();

  /// Show only selected objects
  void onShowOnly(bool theErase);

  /// Process update of a filter item
  void onObjectUpdated();

private:
  /// Update state of button State
  void updateSelectBtn();

  /// Update number of selected objects
  void updateNumberSelected();

  /// Clear selection
  void clearCurrentSelection(bool toUpdate = false);

  /// Update preview
  /// \param theShape a preview shape
  void updatePreview(const TopoDS_Shape& theShape);

  /// Call to redisplay the fiter feature
  void redisplayFeature();

  /// Add a filter by name
  ModuleBase_FilterItem* onAddFilter(const std::string& theFilter);

  /// Return currently created filter items
  QList<ModuleBase_FilterItem*> itemsList() const;

  /// Translate a string
  QString translate(const std::string& theString) const;

  /// Store translated names of filters and their instances
  void storeFilters(const std::list<std::shared_ptr<ModelAPI_Filter> >& theFilters);

private:
  ModuleBase_IWorkshop* myWorkshop;

  QWidget* myFiltersWgt;
  QVBoxLayout* myFiltersLayout;

  QComboBox* myFiltersCombo;
  QPushButton* mySelectBtn;
  QLabel* myNbLbl;
  QCheckBox* myShowBtn;

  /// Type of selection mode
  int mySelectionType;

  /// Result of filtering
  QList<ModuleBase_ViewerPrsPtr> myValues;

  /// A preview shape
  Handle(AIS_Shape) myPreview;

  /// List of displayed objects before "Show only"
  AIS_ListOfInteractive myListIO;

  /// A Feature which will get result of filtering
  FeaturePtr mySelectorFeature;

  /// Attribute name which will get result of filtering
  std::string mySelectorAttribute;

  /// Translated name and the corresponding filter
  std::map<std::string, std::shared_ptr<ModelAPI_Filter> > myFilters;
};


#endif