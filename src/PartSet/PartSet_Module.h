#ifndef PartSet_Module_H
#define PartSet_Module_H

#include "PartSet.h"

#include <ModuleBase_IModule.h>
#include <ModuleBase_Definitions.h>
#include <ModuleBase_ViewerFilters.h>
#include <XGUI_Command.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_CompositeFeature.h>

#include <StdSelect_FaceFilter.hxx>
#include <TopoDS_Shape.hxx>

#include <QMap>
#include <QObject>

#include <string>

#include <memory>

class ModuleBase_Operation;
class ModuleBase_IViewWindow;

class PARTSET_EXPORT PartSet_Module : public ModuleBase_IModule
{
Q_OBJECT

 public:
  PartSet_Module(ModuleBase_IWorkshop* theWshop);
  virtual ~PartSet_Module();

  /// Reads description of features from XML file 
  //virtual void createFeatures();

  /// Called on creation of menu item in desktop
  //virtual void featureCreated(QAction* theFeature);

  //std::string featureFile(const std::string&);

  /// Creates an operation and send it to loop
  /// \param theCmdId the operation name
  //virtual void launchOperation(const QString& theCmdId);

  /// Updates current operation preview, if it has it.
  /// \param theCmdId the operation name
  //void updateCurrentPreview(const std::string& theCmdId);

  /// Creates custom widgets for property panel
  virtual QWidget* createWidgetByType(const std::string& theType, QWidget* theParent,
                                      Config_WidgetAPI* theWidgetApi, std::string theParentId,
                                      QList<ModuleBase_ModelWidget*>& theModelWidgets);

  //XGUI_Workshop* xWorkshop() const;


  /// Returns list of selection modes for the given object for sketch operation
  //static QIntList sketchSelectionModes(ObjectPtr theFeature);

  /// Call back forlast tuning of property panel before operation performance
  virtual void propertyPanelDefined(ModuleBase_Operation* theOperation);

  QStringList sketchOperationIdList() const;

 public slots:
  //void onFeatureTriggered();
  /// SLOT, that is called after the operation is started. Connect on the focus activated signal
  //void onOperationStarted(ModuleBase_Operation* theOperation);
  /// SLOT, that is called after the operation is stopped. Switched off the modfications performed
  /// by the operation start
  //void onOperationStopped(ModuleBase_Operation* theOperation);
  /// SLOT, that is called afetr the popup menu action clicked.
  //void onContextMenuCommand(const QString& theId, bool isChecked);

  /// SLOT, to apply to the current viewer the operation
  /// \param theX the X projection value
  /// \param theY the Y projection value
  /// \param theZ the Z projection value
  //void onPlaneSelected(double theX, double theY, double theZ);

  /// SLOT, to fit all current viewer
  //void onFitAllView();

  //void onRestartOperation(std::string theName, ObjectPtr theFeature);

  /// SLOT, to switch on/off the multi selection in the viewer
  /// \param theEnabled the enabled state
  //void onMultiSelectionEnabled(bool theEnabled);

  /// SLOT, to stop or start selection mode for the features
  /// \param theFeatures a list of features to be disabled
  /// \param theToStop the boolean state whether it it stopped or non stopped
  //void onStopSelection(const QList<ObjectPtr>& theFeatures, const bool isStop);

  /// SLOT, to set selection
  /// \param theFeatures a list of features to be selected
  //void onSetSelection(const QList<ObjectPtr>& theFeatures);

  /// SLOT, Defines Sketch editing mode
  /// \param thePln - plane of current sketch
  //void setSketchingMode(const gp_Pln& thePln);

  /// SLOT, to visualize the feature in another local context mode
  /// \param theFeature the feature to be put in another local context mode
  /// \param theMode the mode appeared on the feature
  //void onFeatureConstructed(ObjectPtr theFeature, int theMode);

  /// Slot which reacts to the point 2d set to the feature. Creates a constraint
  /// \param the feature
  /// \param the attribute of the feature
  //void onStorePoint2D(ObjectPtr theFeature, const std::string& theAttribute);

  /// Called when sketch is launched
  //void onSketchLaunched();

protected slots:
  /// Called when previous operation is finished
  virtual void onOperationComitted(ModuleBase_Operation* theOperation);

  virtual void onOperationAborted(ModuleBase_Operation* theOperation);

  virtual void onOperationStarted(ModuleBase_Operation* theOperation);

  virtual void onOperationStopped(ModuleBase_Operation* theOperation);

  /// Called when previous operation is finished
  virtual void onSelectionChanged();

  /// Called on selection changed event
  //virtual void onSelectionChanged();

  /// SLOT, that is called by mouse press in the viewer.
  /// The mouse released point is sent to the current operation to be processed.
  /// \param theEvent the mouse event
  void onMousePressed(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

  /// SLOT, that is called by mouse release in the viewer.
  /// The mouse released point is sent to the current operation to be processed.
  /// \param theEvent the mouse event
  virtual void onMouseReleased(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);
  
  /// SLOT, that is called by mouse move in the viewer.
  /// The mouse moved point is sent to the current operation to be processed.
  /// \param theEvent the mouse event
  virtual void onMouseMoved(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

  /// SLOT, that is called by the mouse double click in the viewer.
  /// \param theEvent the mouse event
  //virtual void onMouseDoubleClick(QMouseEvent* theEvent);

  /// SLOT, that is called by the key in the viewer is clicked.
  /// \param theEvent the mouse event
  //virtual void onKeyRelease(QKeyEvent* theEvent);

  /// Launches the operation from current highlighting
  void launchEditing();

 protected:
  /// Register validators for this module
  virtual void registerValidators();

  /// Returns new instance of operation object (used in createOperation for customization)
  //virtual ModuleBase_Operation* getNewOperation(const std::string& theFeatureId);

  //! Edits the feature
  //void editFeature(FeaturePtr theFeature);

  //gp_Pln getSketchPlane(FeaturePtr theSketch) const;

 private slots:
   void onVertexSelected(ObjectPtr theObject, const TopoDS_Shape& theShape);

 private:
   /// Converts mouse position to 2d coordinates. 
   /// Member myCurrentSketch has to be correctly defined
  void get2dPoint(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent, 
                  double& theX, double& theY);

  /// Breaks sequense of automatically resterted operations
  void breakOperationSequence();

  //XGUI_Workshop* myWorkshop;
  //PartSet_Listener* myListener;

  //std::map<std::string, std::string> myFeaturesInFiles;

  //Handle(StdSelect_FaceFilter) myPlaneFilter;
  //Handle(ModuleBase_ShapeInPlaneFilter) mySketchFilter;
   QString myLastOperationId;
   FeaturePtr myLastFeature;

   bool myIsDragging;
   bool myDragDone;

   // Automatical restarting mode flag
   bool myRestartingMode;

   double myCurX, myCurY;
   CompositeFeaturePtr myCurrentSketch;
   QList<FeaturePtr> myEditingFeatures;
};

#endif
