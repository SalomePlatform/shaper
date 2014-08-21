// File:        ModuleBase_WidgetEditor.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetEditor_H
#define ModuleBase_WidgetEditor_H

#include <ModuleBase.h>
#include "ModuleBase_WidgetDoubleValue.h"

#include <QObject>
#include <QStringList>

class ModelAPI_Feature;
class QLineEdit;

/**\class ModuleBase_WidgetEditor
 * \ingroup GUI
 * \brief Custom widget. An abstract class to be redefined to fill with some GUI controls
 */
class MODULEBASE_EXPORT ModuleBase_WidgetEditor : public ModuleBase_WidgetDoubleValue
{
Q_OBJECT
 public:
  /// Constructor
  /// \theParent the parent object
  /// \theParent the parent object
  /// \theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetEditor(QWidget* theParent, const Config_WidgetAPI* theData,
                          const std::string& theParentId);
  /// Constructor
  /// \theParent the parent object
  /// \theParent the parent object
  /// \theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetEditor(QWidget* theParent, const std::string& theAttribute);

  /// Destructor
  virtual ~ModuleBase_WidgetEditor();

  /// Set focus to the first control of the current widget. The focus policy of the control is checked.
  /// If the widget has the NonFocus focus policy, it is skipped.
  /// \return the state whether the widget can accept the focus
  virtual bool focusTo();

  /// Creates an editor for the real value and set the new value to the feature
  /// \param theFeature the model feature
  /// \param theAttribute the feature attribute
  static void editFeatureValue(FeaturePtr theFeature, const std::string theAttribute);

 private:
  FeaturePtr myFeature;  ///< the current widget feature
  QStringList myFeatureKinds;  ///< the kinds of possible features
};

#endif
