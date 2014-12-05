#ifndef PartSet_Module_H
#define PartSet_Module_H

#include "PartSet.h"

#include <ModuleBase_IModule.h>
#include <ModuleBase_Definitions.h>
#include <ModuleBase_ViewerFilters.h>
#include <XGUI_Command.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>
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

/// Enumeration to specify the restart operation properties.
enum RestartingMode {
  RM_None, /// the operation should not be restarted
  RM_Forbided, /// the operation should not be restarted after there is no active widget
  RM_LastFeatureUsed, /// the operation is restarted and use the previous feature for own initialization
  RM_EmptyFeatureUsed /// the operation is restarted and does not use the previous feature
};

public:
  PartSet_Module(ModuleBase_IWorkshop* theWshop);
  virtual ~PartSet_Module();

  /// Creates custom widgets for property panel
  virtual QWidget* createWidgetByType(const std::string& theType, QWidget* theParent,
                                      Config_WidgetAPI* theWidgetApi, std::string theParentId,
                                      QList<ModuleBase_ModelWidget*>& theModelWidgets);

  /// Call back forlast tuning of property panel before operation performance
  virtual void propertyPanelDefined(ModuleBase_Operation* theOperation);

  QStringList sketchOperationIdList() const;

public slots:
  /// SLOT, that is called by no more widget signal emitted by property panel
  /// Set a specific flag to restart the sketcher operation
  void onNoMoreWidgets();

protected slots:
  /// Called when previous operation is finished
  virtual void onOperationComitted(ModuleBase_Operation* theOperation);

  virtual void onOperationAborted(ModuleBase_Operation* theOperation);

  virtual void onOperationStarted(ModuleBase_Operation* theOperation);

  virtual void onOperationStopped(ModuleBase_Operation* theOperation);

  /// Called when previous operation is finished
  virtual void onSelectionChanged();

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

  /// SLOT, that is called by key release in the viewer.
  /// The mouse moved point is sent to the current operation to be processed.
  /// \param theEvent the key event
  void onKeyRelease(ModuleBase_IViewWindow* theWnd, QKeyEvent* theEvent);

  /// SLOT, that is called by enter key released
  /// Set a specific type of restarting the current operation
  void onEnterReleased();

  /// Launches the operation from current highlighting
  void launchEditing();

 protected:
  /// Returns new instance of operation object (used in createOperation for customization)
  virtual ModuleBase_Operation* getNewOperation(const std::string& theFeatureId);

  /// Register validators for this module
  virtual void registerValidators();

 private slots:
   void onVertexSelected(ObjectPtr theObject, const TopoDS_Shape& theShape);

   void onPlaneSelected(const std::shared_ptr<GeomAPI_Pln>& thePln);

 private:
   /// Converts mouse position to 2d coordinates. 
   /// Member myCurrentSketch has to be correctly defined
  void get2dPoint(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent, 
                  double& theX, double& theY);

  /// Breaks sequense of automatically resterted operations
  void breakOperationSequence();

 private:
   QString myLastOperationId;
   FeaturePtr myLastFeature;

   bool myIsDragging;
   bool myDragDone;

   // Automatical restarting mode flag
   RestartingMode myRestartingMode;

   double myCurX, myCurY;
   CompositeFeaturePtr myCurrentSketch;
   QList<FeaturePtr> myEditingFeatures;
   QList<AttributePtr> myEditingAttr;

   Handle(ModuleBase_ShapeInPlaneFilter) myPlaneFilter;
  /// A filter which provides selection within a current document or whole PartSet
  Handle(ModuleBase_ShapeDocumentFilter) myDocumentShapeFilter;
};

#endif
