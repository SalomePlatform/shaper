// Copyright (C) 2014-2025  CEA, EDF
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

#ifndef MODULEBASE_WIDGETMULTISELECTOR_H_
#define MODULEBASE_WIDGETMULTISELECTOR_H_

#include <ModuleBase.h>
#include <ModuleBase_WidgetSelector.h>

#include <GeomAPI_Shape.h>
#include <ModelAPI_Result.h>

#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>

#include <QList>
#include <QString>
#include <QStringList>
#include <QPair>
#include <QMap>

class QWidget;
class QCheckBox;
class QPushButton;
class QVBoxLayout;
class ModuleBase_ListView;
class ModuleBase_IWorkshop;
class ModuleBase_ChoiceCtrl;
class ModuleBase_FilterStarter;

/**
* \ingroup GUI
* Implementation of widget for shapes selection. This widget provides selection of several shapes.
* It can be defined in XML file as following:
* \code
* <multi_selector id="group_list" 
*    tooltip="Select a set of objects" 
*    shape_types="Vertices Edges Faces Solids" />
* \endcode
* It uses following parameters:
* - id - is a name of corresponded attribute
* - tooltip - a tooltip for the widget
* - shape_types - list of expected shape types.
*/
class MODULEBASE_EXPORT ModuleBase_WidgetMultiSelector : public ModuleBase_WidgetSelector
{
  Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  ModuleBase_WidgetMultiSelector(QWidget* theParent,
                                 ModuleBase_IWorkshop* theWorkshop,
                                 const Config_WidgetAPI* theData);
  virtual ~ModuleBase_WidgetMultiSelector();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// Update Undo/Redo actions state
  virtual void updateAfterDeactivation();

  /// Update Undo/Redo actions state
  virtual void updateAfterActivation();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValues the wrapped selection values
  /// \param theToValidate a validation of the values flag
  virtual bool setSelection(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                            const bool theToValidate);

  /// Returns values which should be highlighted when the whidget is active
  /// \param theValues a list of presentations
  virtual void getHighlighted(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues);

  /// Returns true if the action can be processed. By default it is empty and returns false.
  /// \param theActionType an action type
  /// \param isActionEnabled if true, the enable state of the action
  virtual bool canProcessAction(ModuleBase_ActionType theActionType, bool& isActionEnabled);

  /// Returns true if the event is processed. The default implementation is empty, returns false.
  virtual bool processAction(ModuleBase_ActionType theActionType,
                             const ActionParamPtr& theParam = ActionParamPtr());

  /// Checks the widget validity. By default, it returns true.
  /// \param thePrs a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  /// Returns list of accessible actions for Undo/Redo commands. By default it returns empty list.
  /// \param theActionType type of action. It can be ActionUndo or ActionRedo.
  virtual QList<ActionInfo> actionsList(ModuleBase_ActionType theActionType) const;

  /// The slot is called when user press Ok or OkPlus buttons in the parent property panel
  virtual void onFeatureAccepted();

  /// Returns True if data of its feature was modified during operation
  virtual bool isModified() const;

  virtual void setReadOnly(bool isReadOnly);

public slots:
  /// Slot is called on selection type changed
  void onSelectionTypeChanged();

protected:
  /// Returns true if envent is processed.
  /// Redefined to process XML state about clear selection in neutral point
  virtual bool processSelection();

protected slots:
  /// Slot for delete command in a list pop-up menu
  void onDeleteItem();

  /// Slot is called on selection of list of selected items
  void onListSelection();

  void onListActivated();

  void onSameTopology(bool theOn);

  void onShowOnly(bool);

  // Slot is called when filters are removed
  void onRemoveFilters();

protected:
  /// Returns true if the event is processed. The default implementation is empty, returns false.
  virtual bool processDelete();

  /// The methiod called when widget is activated
  virtual void activateCustom();

  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// restire type of selection by feature attribute
  virtual bool restoreValueCustom();

  /// Creates an element of the attribute current selection if history is empty
  virtual void appendFirstSelectionInHistory();

  /// Create an element in the history that stores the current selection,
  /// position in the history is incremented
  void appendSelectionInHistory();

  /// Clear list of stored selected items, reset current position into '-1'
  void clearSelectedHistory();

  /// Set the focus on the last item in  the list
  virtual void updateFocus();

  /// Computes and updates name of selected object in the widget
  virtual void updateSelectionName();

  /// Emits model changed info, updates the current control by selection change
  /// \param theDone a state whether the selection is set
  virtual void updateOnSelectionChanged(const bool theDone);

  /// Returns a list of possible shape types
  /// \return a list of shapes
  virtual QIntList shapeTypes() const;

  /// Set current shape type for selection
  void setCurrentShapeType(const QString& theShapeType);

  /// Return the attribute values wrapped in a list of viewer presentations
  /// \return a list of viewer presentations, which contains an attribute result and
  /// a shape. If the attribute do not uses the shape, it is empty
  virtual QList<std::shared_ptr<ModuleBase_ViewerPrs>> getAttributeSelection() const;

  /// Fills the list control by the attribute values
  void updateSelectionList();

  /// Converts the XML defined type choice to the validator type
  /// For example, the "Edges" is converted to "edge"
  std::string validatorType(const QString& theType) const;

  /// Clear selection in the viewer. The content of the widget is cleared also
  void clearSelection();

protected:
  /// Returns attribute indices selected in the widget selection list
  /// \param theIndices a list of indices
  void getSelectedAttributeIndices(std::set<int>& theIndices);

  /// Gets the feature attribute and fill a list of viewer presentation for the attribute
  /// indices. If the the container of indices is empty, it returns all objects.
  /// \param theAttributeIds indices in attribute list to be returned
  /// \param theValues the result presentations, filled with object and shape of an attribute item
  void convertIndicesToViewerSelection(std::set<int> theAttributeIds,
                            QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues) const;

  /// Iterates through the model attribute list and remove elements which do not present in the list
  /// \param theValues the wrapped selection values
  /// \return true if at least one object is removed
  virtual bool removeUnusedAttributeObjects(
    QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues);

  /// Converts viewer presentation selection list to objects and shapes map
  /// \param theValues the wrapped selection values
  /// \return selection list
  std::map<ObjectPtr, std::set<GeomShapePtr> > convertSelection
                                          (QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues);

  /// Returns true if the object and shape present in the container
  /// \param theObject a model object, a set of shapes is searched by it
  /// \param theShape a shape to be in the set of the object shapes
  /// \param theGeomSelection a map built on selection
  /// \param theWorkshop a current workshop
  /// \return boolean result
  static bool findInSelection(const ObjectPtr& theObject,
                            GeomShapePtr theShape,
                            const std::map<ObjectPtr, std::set<GeomShapePtr> >& theGeomSelection,
                            ModuleBase_IWorkshop* theWorkshop);

protected:
  ModuleBase_ListView* myListView; ///< List control

  bool myIsUseChoice; ///< A flag to store use_choice parameter state

  QStringList myShapeTypes; ///< List of Shape types defined in XML

  /// Control for types
  ModuleBase_ChoiceCtrl* myTypeCtrl;

  /// A flag to clear selection by click in empty place in the viewer
  bool myIsNeutralPointClear;

  /// A flag to block set selection perform if the method is in process
  bool myIsSetSelectionBlocked;

  /// A container of selected objects
  QList<QList<std::shared_ptr<ModuleBase_ViewerPrs> > > mySelectedHistoryValues;

  /// Position in a container of selected values
  int myCurrentHistoryIndex;

  bool myIsFirst;
  std::string myDefMode;

  QVBoxLayout* myMainLayout;
  QCheckBox* myGeomCheck;
  ModuleBase_FilterStarter* myFiltersWgt;
  QPushButton* myShowOnlyBtn;
  QPushButton* myRemoveFiltersBtn;

  QObjectPtrList myVisibleObjects;
  QStringList myAllowedObjects;
  QStringList myTmpAllowed;
  std::string myUseFilters;
};

#endif /* MODULEBASE_WIDGETFILESELECTOR_H_ */
