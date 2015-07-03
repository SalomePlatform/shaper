// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetPoint2d.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_WidgetPoint2D_H
#define PartSet_WidgetPoint2D_H

#include "PartSet.h"
#include <ModelAPI_CompositeFeature.h>
#include <ModuleBase_ModelWidget.h>

#include <QObject>

#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>

class ModelAPI_Feature;
class ModuleBase_IWorkshop;
class ModuleBase_ParamSpinBox;
class ModuleBase_IViewWindow;
class GeomAPI_Pnt2d;
class ModuleBase_IWorkshop;
class PartSet_LockApplyMgr;

class QGroupBox;
class QMouseEvent;

/**\class PartSet_WidgetPoint2D
 * \ingroup Modules
 * \brief Implementation of model widget to provide widget to input point 2d
 * In XML can be defined as folloung:
 * \code
 * <sketch-2dpoint_selector id="CircleCenter" title="Center" tooltip="Center coordinates"/>
 * \endcode
 */
class PARTSET_EXPORT PartSet_WidgetPoint2D : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop a current workshop
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  PartSet_WidgetPoint2D(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                        const Config_WidgetAPI* theData, 
                        const std::string& theParentId);
  /// Destructor
  virtual ~PartSet_WidgetPoint2D();

  /// Fills the widget with default values
  virtual bool reset();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValues the wrapped widget values
  virtual bool setSelection(QList<ModuleBase_ViewerPrs>& theValues,
                            const bool theToValidate);

  virtual bool restoreValue();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  //bool initFromPrevious(ObjectPtr theObject);

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// \returns the sketch instance
  CompositeFeaturePtr sketch() const { return mySketch; }

  /// Set sketch instance
  void setSketch(CompositeFeaturePtr theSketch) { mySketch = theSketch; }

  /// Fill the widget values by given point
  /// \param theX the X coordinate
  /// \param theY the Y coordinate
  /// \returns True in case of success
  bool setPoint(double theX, double theY);

  /// Returns coordinate X currently defined in the control
  double x() const;

  /// Returns coordinate Y currently defined in the control
  double y() const;

signals:
  /// Signal about selection of an existing vertex from an object
  void vertexSelected();

public slots:
  /// Process mouse move event
  /// \param theWnd a view window
  /// \param theEvent a mouse event
  void onMouseMove(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

protected slots:
  /// Process mouse release event
  /// \param theWnd a view window
  /// \param theEvent a mouse event
  void onMouseRelease(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

  /// The methiod called when widget is activated
  virtual void activateCustom();

private slots:
  /// Process value changed event
  void onValuesChanged();

 private:
   /// Returns point 2d from selected vertex
   /// \param theView a view window
   /// \param theShape a vertex shape
   /// \param theX an output value of X coordinate
   /// \param theY an output value of Y coordinate
   bool getPoint2d(const Handle(V3d_View)& theView, const TopoDS_Shape& theShape, 
                   double& theX, double& theY) const;

   /// Create a coincidence constraint between the attribute and the parameter object
   /// \theObject a result object
   void setConstraintWith(const ObjectPtr& theObject);

  ModuleBase_IWorkshop* myWorkshop;
  PartSet_LockApplyMgr* myLockApplyMgr; ///< a manager to lock/unlock Apply button in PP

  QGroupBox* myGroupBox;  ///< the parent group box for all intenal widgets
  ModuleBase_ParamSpinBox* myXSpin;  ///< the spin box for the X coordinate
  ModuleBase_ParamSpinBox* myYSpin;  ///< the spin box for the Y coordinate

  //std::string myOptionParam;  /// Parameter name which has to be taken from previous feature

  CompositeFeaturePtr mySketch;
};

#endif
