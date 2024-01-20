// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef XGUI_PROPERTYPANEL_H_
#define XGUI_PROPERTYPANEL_H_

#include "XGUI.h"

#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_IPropertyPanel.h>

#include <QList>

class XGUI_ActionsMgr;
class QKeyEvent;
class QGridLayout;
class QToolButton;
class ModuleBase_PageBase;
class ModuleBase_PageWidget;
class XGUI_OperationMgr;

/// Internal name of property panel widget
MAYBE_UNUSED const static char* PROP_PANEL = "property_panel_dock";

/// Internal name of Ok button
MAYBE_UNUSED const static char* PROP_PANEL_OK = "property_panel_ok";

/// Internal name of Ok button
MAYBE_UNUSED const static char* PROP_PANEL_OK_PLUS = "property_panel_ok_plus";

/// Internal name of Cancel button
MAYBE_UNUSED const static char* PROP_PANEL_CANCEL = "property_panel_cancel";

/// Internal name of Help button
MAYBE_UNUSED const static char* PROP_PANEL_HELP = "property_panel_help";

/// Internal name of Preview button
MAYBE_UNUSED const static char* PROP_PANEL_PREVIEW = "property_panel_preview";

/**
* \ingroup GUI
* Realization of Property panel object.
*/
class XGUI_EXPORT XGUI_PropertyPanel : public ModuleBase_IPropertyPanel
{
Q_OBJECT
 public:

  /// Constructor
  /// \param theParent is a parent of the property panel
  /// \param theMgr operation manager
  XGUI_PropertyPanel(QWidget* theParent, XGUI_OperationMgr* theMgr);

  virtual ~XGUI_PropertyPanel();

  /// Returns header widget
  virtual QWidget* headerWidget() const { return myHeaderWidget; }

  /// Returns main widget of the property panel, which children will be created
  /// by WidgetFactory using the XML definition
  ModuleBase_PageBase* contentWidget();

  /// Brings back all widget created by widget factory for signal/slot
  /// connections and further processing
  void setModelWidgets(const QList<ModuleBase_ModelWidget*>& theWidgets);

  /// Returns all property panel's widget created by WidgetFactory
  virtual const QList<ModuleBase_ModelWidget*>& modelWidgets() const;

  /// Removes all widgets in the widget area of the property panel
  virtual void cleanContent();

  /// Returns currently active widget. This is a widget from internal container of widgets
  /// (myWidgets) activated/deactivated by focus in property panel. If parameter is true,
  /// the method finds firstly the custom widget, after the direct active widget.
  /// \param isUseCustomWidget boolean state if the custom widget might be a result
  virtual ModuleBase_ModelWidget* activeWidget(const bool isUseCustomWidget = false) const;

  /// Activate the next widget in the property panel
  /// \param theWidget a widget. The next widget should be activated
  virtual void activateNextWidget(ModuleBase_ModelWidget* theWidget);

  /// Activate the next from current widget in the property panel
  virtual void activateNextWidget();

  /// Set focus on the Ok button
  virtual void setFocusOnOkButton();

  /// Set Enable/Disable state of Cancel button
  /// \param theEnabled Enable/Disable state of Cancel button
  virtual void setCancelEnabled(bool theEnabled);

  /// \return Enable/Disable state of Cancel button
  virtual bool isCancelEnabled() const;

  /// Editing mode depends on mode of current operation. This value is defined by it.
  /// \param isEditing state of editing mode flag
  virtual void setEditingMode(bool isEditing);

  //! Allows to set predefined actions for the property panel fetched from the ActionsMgr
  void setupActions(XGUI_ActionsMgr* theMgr);

  /// Returns widget processed by preselection
  virtual ModuleBase_ModelWidget* preselectionWidget() const;

  /// Sets widget processed by preselection
  virtual void setPreselectionWidget(ModuleBase_ModelWidget* theWidget);

  /// Returns operation manager
  XGUI_OperationMgr* operationMgr() const { return myOperationMgr; }

  /// Find under the panel a child button with the parameter name
  /// \param theInternalName a button object name
  /// \return button instance or NULL
  QToolButton* findButton(const char* theInternalName) const;

  /// Possibility to process focus by method, for example when Tab or SHIF+Tab is pressed
  /// but property panel is not active widget
  /// \param theIsNext true, if Tab(to the next widget) or false(moving to the previous)
  bool setFocusNextPrevChild(bool theIsNext);

  /// The method is called on accepting of operation
  virtual void onAcceptData();

  /// Set internal active widget, that can be returned as active widget and participate in active
  /// selection filters/modes in application. It emits signal about property panel activation or
  /// deactivation and updates selection filters/modes accordingly.
  /// \param theWidget a widget control to store as internal active widget
  void setInternalActiveWidget(ModuleBase_ModelWidget* theWidget);

  void updateApplyPlusButton(FeaturePtr theFeature);

public slots:
  /// \brief Update all widgets in property panel with values from the given feature
  /// \param theFeature a Feature to update values in widgets
  void updateContentWidget(FeaturePtr theFeature);

  /// \brief If the XML definition of the feature contains information about specific
  /// content of the property panel, it creates the panel and allow filling it by the given feature
  /// \param theFeature a Feature to fill property panel
  void createContentPanel(FeaturePtr theFeature);

  /**
  * Makes the given widget active, highlights it and removes
  * highlighting from the previous active widget
  * \param theWidget which has to be activated
  * \param theEmitSignal a flag to prohibit signal emit
  */
  virtual void activateWidget(ModuleBase_ModelWidget* theWidget, const bool theEmitSignal = true);

   /// Activates the parameter widget if it can accept focus
   /// \param theWidget a widget where focus in event happened
  void onFocusInWidget(ModuleBase_ModelWidget* theWidget);


  /// Activate next widget
  /// \param theWidget the current widget
  void onActivateNextWidget(ModuleBase_ModelWidget* theWidget);

signals:
  /// The signal is emitted if the enter is clicked in the control of the widget
  /// \param theObject a sender of the event
  void enterClicked(QObject* theObject);

  /// Emits on deactivating property panel (e.g. by clearContent)
  void propertyPanelDeactivated();
  /// Emits on widget activating
  void propertyPanelActivated();

protected:
  /// Makes the widget active, deactivate the previous, activate and hightlight the given one
  /// \param theWidget a widget
  /// \param isEmitSignal flag whether panel signals should be emitted
  bool setActiveWidget(ModuleBase_ModelWidget* theWidget, const bool isEmitSignal);
  /// The parent method that processes the "Tab"/"SHIF + Tab" keyboard events
  /// Emits a signal about focus change
  /// If theIsNext is true, this function searches forward, if next is false, it searches backward.
  virtual bool focusNextPrevChild(bool theIsNext);
  /// Activate the next widget in the property panel
  /// \param theWidget a widget. The next widget should be activated
  /// \param isCheckVisibility flag whether the next widget visibility is checked
  void activateNextWidget(ModuleBase_ModelWidget* theWidget,
                          const bool isCheckVisibility);

 protected:
   /// A method called on the property panel closed
   /// \param theEvent a close event
   void closeEvent(QCloseEvent* theEvent);

private:
  QWidget* myHeaderWidget;  ///< A header widget

  ModuleBase_PageWidget* myPanelPage;
  QList<ModuleBase_ModelWidget*> myWidgets;

  /// Currently active widget
  ModuleBase_ModelWidget* myActiveWidget;
  /// Currently widget processed by preselection
  ModuleBase_ModelWidget* myPreselectionWidget;
  /// Some custom widget set from outside
  ModuleBase_ModelWidget* myInternalActiveWidget;

  XGUI_OperationMgr* myOperationMgr;
};

#endif /* XGUI_PROPERTYPANEL_H_ */
