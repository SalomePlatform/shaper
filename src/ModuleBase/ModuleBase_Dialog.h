// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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
