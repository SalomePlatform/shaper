// File:        ModuleBase_WidgetEditor.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetEditor_H
#define ModuleBase_WidgetEditor_H

#include <ModuleBase.h>
#include "ModuleBase_ModelWidget.h"

#include <QObject>
#include <QStringList>

class ModelAPI_Feature;
class QLineEdit;

/**\class ModuleBase_WidgetEditor
 * \ingroup GUI
 * \brief Custom widget. An abstract class to be redefined to fill with some GUI controls
 */
class MODULEBASE_EXPORT ModuleBase_WidgetEditor : public ModuleBase_ModelWidget
{
  Q_OBJECT
public:
  /// Constructor
  /// \theParent the parent object
  /// \theParent the parent object
  /// \theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetEditor(QWidget* theParent, const Config_WidgetAPI* theData);
  /// Destructor
  virtual ~ModuleBase_WidgetEditor();

  /// Saves the internal parameters to the given feature
  /// \param theFeature a model feature to be changed
  virtual bool storeValue(FeaturePtr theFeature) const;

  virtual bool restoreValue(FeaturePtr theFeature);

  /// Set focus to the first control of the current widget. The focus policy of the control is checked.
  /// If the widget has the NonFocus focus policy, it is skipped.
  virtual void focusTo();

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const;

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

protected slots:
  /// Slot to check the editing stop
  void onStopEditing();

private:
  QLineEdit* myEditor;
  FeaturePtr myFeature; ///< the current widget feature
  QStringList myFeatureKinds; ///< the kinds of possible features
};

#endif
