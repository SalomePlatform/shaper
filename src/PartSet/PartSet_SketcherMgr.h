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

#ifndef PartSet_SketcherMgr_H
#define PartSet_SketcherMgr_H

#include "PartSet.h"

#include "PartSet_PreviewSketchPlane.h"
#include "PartSet_Tools.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Result.h>
#include <Events_Listener.h>
#include <ModuleBase_SelectionFilterType.h>

#include <ModuleBase_Definitions.h>
#include <ModuleBase_ModelWidget.h>

#include <GeomAPI_Pln.h>
#include <GeomAPI_AISObject.h>

#ifdef HAVE_SALOME
  #include <OCCViewer_ViewModel.h>
#else
  #include <AppElements_Viewer.h>
#endif


#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <SelectMgr_ListOfFilter.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <AIS_Shape.hxx>

#include <QObject>
#include <QList>
#include <QMap>

#include <set>

class PartSet_Module;
class ModuleBase_IViewWindow;
class ModuleBase_ModelWidget;
class ModuleBase_Operation;
class XGUI_OperationMgr;
class XGUI_Workshop;
class XGUI_Displayer;
class PartSet_ExternalPointsMgr;

class AIS_InteractiveObject;

class QMouseEvent;
class PartSet_SketcherMgr;

#ifdef HAVE_SALOME
class PartSet_Fitter : public OCCViewer_Fitter
#else
class PartSet_Fitter : public AppElements_Fitter
#endif
{
public:
  PartSet_Fitter(PartSet_SketcherMgr* theSketchMgr):
    mySketchMgr(theSketchMgr) {}

  /// A method which has top be reimplemented to provide alterantive implementation FitAll command
  /// \param theView - a view which has to be fit
  virtual void fitAll(Handle(V3d_View) theView);

private:
  PartSet_SketcherMgr* mySketchMgr;
};



/**
* \ingroup Modules
* A class for management of sketch operations
  At the time of the sketcher operation active, only the sketch sub-feature results are
  displayed in the viewer. After the sketch create/edit operation is finished, the sub-feature
  are hidden, the sketch feature result is displayed
*/
class PARTSET_EXPORT PartSet_SketcherMgr : public QObject, public Events_Listener
{
  Q_OBJECT
  /// Struct to define gp point, with the state is the point is initialized
  struct Point
  {
    /// Constructor
    Point()
    {
      myIsInitialized = false;
    }
    /// Destructor
    ~Point()
    {
    }

    /// clear the initialized flag.
    void clear()
    {
      myIsInitialized = false;
    }
    /// set the point and switch on the initialized flag
    /// \param thePoint the point
    void setValue(const double theX, const double theY)
    {
      myIsInitialized = true;
      myCurX = theX;
      myCurY = theY;
    }

    bool myIsInitialized;  /// the state whether the point is set
    double myCurX, myCurY; /// the point coordinates
  };

public:
  /// Struct to define selection model information to store/restore selection
  struct SelectionInfo
  {
    std::set<AttributePtr> myAttributes; /// the selected attributes
    std::set<ResultPtr> myResults; /// the selected results
    TopoDS_Shape myFirstResultShape; /// the first shape of feature result
    TopTools_MapOfShape myLocalSelectedShapes; /// shapes of local selection
  };
  typedef QMap<FeaturePtr, SelectionInfo> FeatureToSelectionMap;

public:
  /// Constructor
  /// \param theModule a pointer to PartSet module
  PartSet_SketcherMgr(PartSet_Module* theModule);

  virtual ~PartSet_SketcherMgr();

  /// Returns true if the operation is the sketch
  /// \param theOperation an operation
  /// \return the boolean result
  static bool isSketchOperation(ModuleBase_Operation* theOperation);

  /// Returns true if the operation feature belongs to list of granted features of Sketch
  /// operation. An operation of a sketch should be started before.
  /// \param theOperation an operation
  /// \return the boolean result
  bool isNestedSketchOperation(ModuleBase_Operation* theOperation) const;

  /// Returns true if the feature kind belongs to list of granted features of Sketch
  /// operation. An operation of a sketch should be started before.
  /// \param theOperation an operation
  /// \return the boolean result
  bool isNestedSketchFeature(const QString& theFeatureKind) const;

  /// Returns true if the operation is a create and nested sketch operationn
  /// \param theOperation a checked operation
  /// \param theSketch a sketch feature
  //// \return boolean value
  bool isNestedCreateOperation(ModuleBase_Operation* theOperation,
                                      const CompositeFeaturePtr& /*theSketch*/) const;

  /// Returns true if the operation is an edit nested feature one
  /// \param theOperation a checked operation
  //// \return boolean value
  bool isNestedEditOperation(ModuleBase_Operation* theOperation,
                                    const CompositeFeaturePtr& /*theSketch*/) const;

  /// Returns whether the current operation is a sketch entity - line, point, arc or circle
  /// \param theId is an id of object
  /// \return a boolean value
  static bool isEntity(const std::string& theId);

  /// Returns whether the feature has external attribute filled with 'true' value
  /// \param theFeature a feature object
  /// \return a boolean value
  static bool isExternalFeature(const FeaturePtr& theFeature);

  /// Returns whether the current operation is a sketch distance - lenght, distance or radius
  /// \param theOperation the operation
  /// \return a boolean value
  static bool isDistanceOperation(ModuleBase_Operation* theOperation);

  /// Returns whether the feature kind is a sketch distance - lenght, distance or radius
  /// \param theKind the feature kind
  /// \return a boolean value
  static bool isDistanceKind(std::string& theKind);

  /// Returns true if a mouse cursor is over viewer window
  bool isMouseOverWindow() { return myIsMouseOverWindow; }

  /// Returns current Sketch feature/ Returns NULL if there is no launched sketch operation
  CompositeFeaturePtr activeSketch() const { return myCurrentSketch; }

  /// Returns help class to visualize sketcher plane
  /// \return a preview plane
  PartSet_PreviewSketchPlane* previewSketchPlane() const { return mySketchPlane; }

  /// Starts sketch operation
  void startSketch(ModuleBase_Operation* );

  /// Stops sketch operation
  void stopSketch(ModuleBase_Operation* );

  /// Starts sketch operation, connects to the opeation property panel
  /// \param theOperation a committed operation
  //void startNestedSketch(ModuleBase_Operation* theOperation);

  /// Stop sketch operation, disconnects from the opeation property panel
  /// \param theOperation a stopped operation
  void stopNestedSketch(ModuleBase_Operation* theOperation);

  /// Visualizes the operation feature if it is a creation nested feature operation
  /// \param theOperation a committed operation
  void commitNestedSketch(ModuleBase_Operation* theOperation);

  /// Returns true if the filter is created by the sketch manager
  /// \param theFilterType a checked type
  /// \return boolean value
  bool sketchSelectionFilter(const ModuleBase_SelectionFilterType theFilterType);

  /// Append selection filter into the module and type of the filter in internal container
  /// \param theFilterType selection filter type
  /// \param theFilter added filter
  void registerSelectionFilter(const ModuleBase_SelectionFilterType theFilterType,
                               const Handle(SelectMgr_Filter)& theFilter);

  /// Commit the operation if it is possible. If the operation is dimention constraint,
  /// it gives widget editor to input dimention value
  /// \return true if the operation is stopped after activation
  bool operationActivatedByPreselection();

  /// Returns True if there are available Undos and the sketch manager allows undo
  /// \return the boolean result
  bool canUndo() const;

  //! Returns True if there are available Redos and the sketch manager allows redo
  /// \return the boolean result
  bool canRedo() const;

  /// Returns False only if the sketch creating feature can not be visualized.
  /// \return a boolean value
  //bool canCommitOperation() const;

  /// Returns whether the object can be erased at the bounds of the active operation.
  /// Sketch sub-entities can not be erased during the sketch operation
  /// \param theObject a model object
  bool canEraseObject(const ObjectPtr& theObject) const;

  /// Returns whether the object can be displayed at the bounds of the active operation.
  /// Display only current operation results for usual operation and ask the sketcher manager
  /// if it is a sketch operation
  /// \param theObject a model object
  bool canDisplayObject(const ObjectPtr& theObject) const;

  /// Returns whether the constraint object can be displayed. It depends on the sketch check
  /// box states
  /// \param theObject a model object
  /// \param theState the constraint visible state state to be checked
  /// \param isProcessed an output parameter if it is processed
  /// \return result value
  bool canDisplayConstraint(const FeaturePtr& theObject,
                            const PartSet_Tools::ConstraintVisibleState& theState,
                            bool& isProcessed) const;

  /// Check the given objects either there are some results of the current sketch. If so,
  /// it suggests to delete them as there are no functionality to show back hidden sketch objects
  /// \param theObjects a list of hidden objects
  //virtual void processHiddenObject(const std::list<ObjectPtr>& theObjects);

  /// Returns true if the mouse is over viewer or property panel value is changed
  /// \return boolean result
  bool canDisplayCurrentCreatedFeature() const;

  /// Returns true if the current operation is nested creation or internal reentrant edit
  /// \param theOperation an operation
  bool canChangeCursor(ModuleBase_Operation* theOperation) const;

  /// Returns state of constraints showing flag
  const QMap<PartSet_Tools::ConstraintVisibleState, bool>& showConstraintStates();

  /// Returns true if the object is a current sketch sub feature of a result of the feature
  /// \param theObject an object
  /// \return boolean value
  bool isObjectOfSketch(const ObjectPtr& theObject) const;

  /// Enumeration for selection mode used
  enum SelectionType {
    ST_HighlightType, /// Only highlighted objects
    ST_SelectType, /// Only selected objects
    ST_SelectAndHighlightType /// Both, highlighted and selected objects
  };

  /// Saves the current selection in the viewer into an internal container
  /// It obtains the selected attributes.
  /// The highlighted objects can be processes as the selected ones
  /// \param theHighlightedOnly a boolean flag
  /// \param theCurrentSelection a container filled by the current selection
  void storeSelection(const SelectionType theType, FeatureToSelectionMap& theCurrentSelection);

  /// Restores previously saved selection state
  /// \param theCurrentSelection a container filled by the current selection
  void restoreSelection(FeatureToSelectionMap& theCurrentSelection);

  /// Return error state of the sketch feature, true if the error has happened
  /// \return boolean value
  bool sketchSolverError();

  //! Returns the feature error if the current state of the feature in the sketch is not correct
  //! If the feature is correct, it returns an empty value
  //! Incorrect states: the feature is sketch, the solver error value
  //! The feature value is reset, this is the flag of sketch mgr
  //! \return string value
  QString getFeatureError(const FeaturePtr& theFeature);

  /// It nullify internal flags concerned to clicked mouse event
  void clearClickedFlags();

  /// Returns list of strings which contains id's of sketch replication operations
  static const QStringList& replicationsIdList();

  /// Returns list of strings which contains id's of constraints operations
  static const QStringList& constraintsIdList();

  /// Returns a list of modes, where the AIS objects should be activated
  /// \param theSketch a sketch object, modes are empty if sketch plane is not defined yet
  /// \param theModes a list of modes
  static void sketchSelectionModes(const CompositeFeaturePtr& theSketch, QIntList& theModes);

  /// Create specific for the module presentation
  /// \param theResult an object for presentation
  /// \return created presentation or NULL(default value)
  Handle(AIS_InteractiveObject) createPresentation(const ObjectPtr& theResult);

  /// Connects or disconnects to the value changed signal of the property panel widgets
  /// \param theWidget a property contol widget
  /// \param isToConnect a boolean value whether connect or disconnect
  void connectToPropertyPanel(ModuleBase_ModelWidget* theWidget, const bool isToConnect);

  /// Visualize the operation feature if the previous state is modified value in property panel
  /// \param thePreviousState the previous widget value state
  void widgetStateChanged(int thePreviousState);

  /// If the current operation is a dimention one, the style of dimension visualization is send for
  /// the current object
  /// \param theObject an object to be customized
  //void customisePresentation(const ObjectPtr& theObject);

  void customizeSketchPresentation(const ObjectPtr& theObject, const AISObjectPtr& thePrs) const;

  /// Update sketch presentations according to the the state
  /// \param theType a type of sketch visualization style
  /// \param theState a boolean state
  void updateBySketchParameters(const PartSet_Tools::ConstraintVisibleState& theType,
                                bool theState);

  bool isShowFreePointsShown() const {
    return myPointsHighlight.size() > 0;
  }

  PartSet_Module* module() const { return myModule; }

  /** \brief Implementation of Event Listener method
  *  \param[in] theMessage the data of the event
  */
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  bool isSketchStarted() const { return myIsSketchStarted; }


public slots:
  /// Process sketch plane selected event
  void onPlaneSelected(const std::shared_ptr<GeomAPI_Pln>& thePln);

  /// The slot is called when user checks "Show free points" button
  /// \param toShow a state of the check box
  void onShowPoints(bool toShow);

private slots:
  /// Toggle show constraints
  void onShowConstraintsToggle(int theType, bool theState);
  /// Process the enter mouse to the view port. If the current operation is a create of
  /// a nested sketch feature, it updates internal flags to display the feature on mouse move
  void onEnterViewPort();
  /// Process the leave mouse of the view port. If the current operation is a create of
  /// a nested sketch feature, it hides the feature in the viewer
  void onLeaveViewPort();
  /// Listens to the value changed signal and display the current operation feature
  //void onBeforeValuesChangedInPropertyPanel();
  /// Listens to the signal about the modification of the values
  /// have been done in the property panel
  //void onAfterValuesChangedInPropertyPanel();

  void onMousePressed(ModuleBase_IViewWindow*, QMouseEvent*);
  void onMouseReleased(ModuleBase_IViewWindow*, QMouseEvent*);
  void onMouseMoved(ModuleBase_IViewWindow*, QMouseEvent*);
  void onMouseDoubleClick(ModuleBase_IViewWindow*, QMouseEvent*);
  void onApplicationStarted();
  //void onBeforeWidgetActivated(ModuleBase_ModelWidget* theWidget);

  void onBeforeContextMenu();
  void onAfterContextMenu();

private:
  /// Launches the operation from current highlighting
  void launchEditing();

  /// Converts mouse position to 2d coordinates.
  /// Member myCurrentSketch has to be correctly defined
  void get2dPoint(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent,
                  Point& thePoint);

  /// Show distance value editor if it is a distance operation and all attribute references
  /// are filled by preseletion
  /// \return true if the value is accepted
  static bool setDistanceValueByPreselection(ModuleBase_Operation* theOperation,
                                             ModuleBase_IWorkshop* theWorkshop,
                                             bool& theCanCommitOperation);

  /// Applyes the current selection to the object in the workshop viewer
  /// It includes the selection in all modes of activation, even local context - vertexes, edges
  /// It gets all results of the feature, find an AIS object in the viewer and takes all BRep
  /// selection owners. If the owner is vertex, the corresponded attribute is seached in
  /// the feature and if it is in the container of selected attributes, the owner is put in the
  /// out container. If the owner is edge and the current result is in the container of selected
  /// results, the owner is put in the out container.
  /// \param theFeature a feature or result object
  /// \param theSketch a current sketch feature
  /// \param theWorkshop a workshop to have an access to AIS context and displayer
  /// \param theSelection a container of the selection, it has results and attributres for a feature
  /// \param theOwnersToSelect an out container of found owners
  static void getSelectionOwners(const FeaturePtr& theFeature,
                                  const FeaturePtr& theSketch,
                                  ModuleBase_IWorkshop* theWorkshop,
                                  const FeatureToSelectionMap& theSelection,
                                  SelectMgr_IndexedMapOfOwner& theOwnersToSelect);

  /// Returns true if the created feature is visible
  /// \param
  bool isVisibleCreatedFeature() const;

  /// Returns a current operation
  /// \return an operation
  ModuleBase_Operation* getCurrentOperation() const;

  /// Get the active widget in the property panel
  ModuleBase_ModelWidget* getActiveWidget() const;

  /// Erase or display the feature of the current operation. If the mouse over the active view or
  /// a current value is changed by property panel, the feature is displayed otherwise it is hidden
  /// \param theOperation an operation which feature is to be displayed,
  ///                     it is nested create operation
  /// \param isToDisplay a flag about the display or erase the feature
  void visualizeFeature(const FeaturePtr& theFeature, const bool isEditOperation,
                        const bool isToDisplay, const bool isFlushRedisplay = true);

private:
  /// Updates selection priority of the presentation
  /// \param theObject object to find a presentation which will be corrected
  /// \param theFeature a feature of the presentation
  void updateSelectionPriority(ObjectPtr theObject, FeaturePtr theFeature);
  /// Returns current workshop
  XGUI_Workshop* workshop() const;
  /// Returns operation manager
  XGUI_OperationMgr* operationMgr() const;

  std::vector<int> colorOfObject(const ObjectPtr& theObject,
    const FeaturePtr& aFeature, bool isConstruction) const;

private:
  PartSet_Module* myModule;
  PartSet_PreviewSketchPlane* mySketchPlane; // display/erase sketch plane on start/stop sketch

  bool myPreviousDrawModeEnabled; // the previous selection enabled state in the viewer
  bool myIsEditLaunching;
  bool myIsDragging;
  bool myDragDone;
  bool myIsMouseOverWindow; /// the state that the mouse over the view
  /// the state whether the over view state is processed by mouseMove method
  bool myIsMouseOverViewProcessed;
  bool myIsPopupMenuActive; /// the state of the popup menu is shown
  Point myCurrentPoint;
  //Point myClickedPoint;

  CompositeFeaturePtr myCurrentSketch;

  std::set<ModuleBase_SelectionFilterType> mySelectionFilterTypes;

  FeatureToSelectionMap myCurrentSelection;
  bool myPreviousUpdateViewerEnabled;

  QMap<PartSet_Tools::ConstraintVisibleState, bool> myIsConstraintsShown;

  PartSet_ExternalPointsMgr* myExternalPointsMgr;

  QMap<ResultPtr, Handle(AIS_Shape)> myPointsHighlight;

  bool myNoDragMoving;

  bool myIsSketchStarted;
};


#endif