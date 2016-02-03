// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  ModuleBase_WidgetEditor(QWidget* theParent, const Config_WidgetAPI* theData,
                          const std::string& theParentId);
  /// Constructor
  /// \param theParent the parent object
  /// \param theAttribute The attribute of the model widget
  ModuleBase_WidgetEditor(QWidget* theParent, const std::string& theAttribute);

  /// Destructor
  virtual ~ModuleBase_WidgetEditor();

  /// Set focus to the first control of the current widget. The focus policy of the control is checked.
  /// If the widget has the NonFocus focus policy, it is skipped.
  /// \return the state whether the widget can accept the focus
  virtual bool focusTo();

  /// Shous popup window under cursor for data editing
  /// \param theSendSignals a flag whether the signals should be sent or the value
  /// is to be applyed directly
  void showPopupEditor(const bool theSendSignals = true);

  void setCursorPosition(const int theX, const int theY);

private:
   void editedValue(double& outValue, QString& outText);

 private:
   ///< the current widget feature
   FeaturePtr myFeature;  

   ///< the kinds of possible features
   QStringList myFeatureKinds;  

   int myXPosition, myYPosition;
};

#endif
