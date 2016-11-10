// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef ModuleBase_Dialog_H
#define ModuleBase_Dialog_H

#include "ModuleBase.h"
#include <ModelAPI_Feature.h>

#include <QDialog>
#include <string>


class ModuleBase_IWorkshop;
class ModuleBase_ModelWidget;
class QDialogButtonBox;


/**
 * \ingroup GUI
 * A dialog box which is used for modal dialog box feature interface
 */
class ModuleBase_Dialog : public QDialog
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent a workshop object instance
  /// \param theId an Id of a feature
  /// \param theDescription an XML description of the feature
  ModuleBase_Dialog(ModuleBase_IWorkshop* theParent, const QString& theId,
                    const std::string& theDescription);

  /// Redefinition of virtual method
  virtual void accept();

protected:
  /// Redefinition of virtual method
  virtual void showEvent(QShowEvent* theEvent);

private:
  /// Initialising of the widget
  void initializeWidget(ModuleBase_ModelWidget* theWidget);

  /// Id of the feature
  QString myId;

  /// XML description of the feature
  std::string myDescription;

  /// An instance of workshop object
  ModuleBase_IWorkshop* myWorkshop;

  /// The feature instance
  FeaturePtr myFeature;

  /// List of created widgets
  QList<ModuleBase_ModelWidget*> myWidgets;

  /// Pointer on active widget
  ModuleBase_ModelWidget* myActiveWidget;

  /// Buttons of the dialog
  QDialogButtonBox* myButtonsBox;
};

#endif
