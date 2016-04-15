// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef ModuleBase_Dialog_H
#define ModuleBase_Dialog_H

#include "ModuleBase.h"
#include <ModelAPI_Feature.h>

#include <QDialog>
#include <string>


class ModuleBase_IWorkshop;
class ModuleBase_ModelWidget;

/**
 * \ingroup GUI
 * A dialog box which is used for modal dialog box feature interface
 */
class ModuleBase_Dialog : public QDialog
{
  Q_OBJECT
public:
  ModuleBase_Dialog(ModuleBase_IWorkshop* theParent, const QString& theId, 
                    const std::string& theDescription);

protected:
  virtual void showEvent(QShowEvent* theEvent);


private:
  void initializeWidget(ModuleBase_ModelWidget* theWidget);

  QString myId;
  std::string myDescription;
  ModuleBase_IWorkshop* myWorkshop;
  FeaturePtr myFeature;
  QList<ModuleBase_ModelWidget*> myWidgets;
};

#endif
