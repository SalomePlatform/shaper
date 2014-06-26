// File:        ModuleBase_WidgetPoint2D.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetPoint2D_H
#define ModuleBase_WidgetPoint2D_H

#include <ModuleBase.h>
#include "ModuleBase_ModelWidget.h"

#include <QObject>

class ModelAPI_Feature;
class ModuleBase_WidgetValue;
class GeomAPI_Pnt2d;

class QGroupBox;
class QDoubleSpinBox;

/**\class ModuleBase_WidgetPoint2D
 * \ingroup GUI
 * \brief Custom widget. An abstract class to be redefined to fill with some GUI controls
 */
class MODULEBASE_EXPORT ModuleBase_WidgetPoint2D : public ModuleBase_ModelWidget
{
  Q_OBJECT
public:
  /// Constructor
  /// \theParent the parent object
  /// \theParent the parent object
  /// \theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetPoint2D(QWidget* theParent, const Config_WidgetAPI* theData);
  /// Destructor
  virtual ~ModuleBase_WidgetPoint2D();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValue the wrapped widget value
  virtual bool setValue(ModuleBase_WidgetValue* theValue);

  /// Saves the internal parameters to the given feature
  /// \param theFeature a model feature to be changed
  virtual bool storeValue(FeaturePtr theFeature) const;

  virtual bool restoreValue(FeaturePtr theFeature);

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const;

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Process key release envent on the widget spin box controls
  /// \param theObject the object where the event happens
  /// \param theEvent the processed event
  virtual bool eventFilter(QObject *theObject, QEvent *theEvent);

  bool initFromPrevious(FeaturePtr theFeature);

signals:
  /// Signal about the point 2d set to the feature
  /// \param the feature
  /// \param the attribute of the feature
  void storedPoint2D(FeaturePtr theFeature, const std::string& theAttribute);

protected:
  /// Fill the widget values by given point
  /// \param thePoint the point
  void setPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint);

private:
  QGroupBox* myGroupBox; ///< the parent group box for all intenal widgets
  QDoubleSpinBox* myXSpin; ///< the spin box for the X coordinate
  QDoubleSpinBox* myYSpin; ///< the spin box for the Y coordinate

  std::string myOptionParam; /// Parameter name which has to be taken from previous feature
};

#endif
