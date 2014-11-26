// File:        PartSet_WidgetPoint2D.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_WidgetPoint2D_H
#define PartSet_WidgetPoint2D_H

#include "PartSet.h"
#include <ModelAPI_CompositeFeature.h>
#include <ModuleBase_ModelWidget.h>

#include <QObject>

#include <TopoDS_Shape.hxx>

class ModelAPI_Feature;
class ModuleBase_WidgetValue;
class ModuleBase_IWorkshop;
class ModuleBase_DoubleSpinBox;
class ModuleBase_IViewWindow;
class GeomAPI_Pnt2d;
class XGUI_Workshop;

class QGroupBox;
class QMouseEvent;

/**\class PartSet_WidgetPoint2D
 * \ingroup GUI
 * \brief Custom widget. An abstract class to be redefined to fill with some GUI controls
 */
class PARTSET_EXPORT PartSet_WidgetPoint2D : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \theParent the parent object
  /// \theParent the parent object
  /// \theData the widget configuation. The attribute of the model widget is obtained from
  PartSet_WidgetPoint2D(QWidget* theParent, const Config_WidgetAPI* theData, 
                        const std::string& theParentId);
  /// Destructor
  virtual ~PartSet_WidgetPoint2D();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValue the wrapped widget value
  virtual bool setValue(ModuleBase_WidgetValue* theValue);

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

  //bool initFromPrevious(ObjectPtr theObject);

  /// The methiod called when widget is activated
  virtual void activate();

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  XGUI_Workshop* workshop() const { return myWorkshop; }

  void setWorkshop(XGUI_Workshop* theWork) { myWorkshop = theWork; }

  /// \returns the sketch instance
  CompositeFeaturePtr sketch() const { return mySketch; }
  void setSketch(CompositeFeaturePtr theSketch) { mySketch = theSketch; }

  /// Fill the widget values by given point
  /// \param thePoint the point
  void setPoint(double theX, double theY);

  /// Returns coordinate X currently defined in the control
  double x() const;

  /// Returns coordinate Y currently defined in the control
  double y() const;

signals:
  /// Signal about selection of an existing vertex from an object
  /// \param theObject - the selected object
  /// \param theShape - the selected shape
  void vertexSelected(ObjectPtr theObject, const TopoDS_Shape& theShape);

protected slots:
  void onMouseRelease(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);
  void onMouseMove(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent);

 private:
  XGUI_Workshop* myWorkshop;

  QGroupBox* myGroupBox;  ///< the parent group box for all intenal widgets
  ModuleBase_DoubleSpinBox* myXSpin;  ///< the spin box for the X coordinate
  ModuleBase_DoubleSpinBox* myYSpin;  ///< the spin box for the Y coordinate

  //std::string myOptionParam;  /// Parameter name which has to be taken from previous feature

  CompositeFeaturePtr mySketch;
};

#endif
