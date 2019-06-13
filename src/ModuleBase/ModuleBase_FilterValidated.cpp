// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "ModuleBase_FilterValidated.h"
#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_IViewer.h"

#include <ModuleBase_IModule.h>
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_WidgetValidated.h>
#include <ModuleBase_WidgetValidator.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_Tools.h>

IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_FilterValidated, SelectMgr_Filter);

Standard_Boolean ModuleBase_FilterValidated::
  IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  bool aValid = true;
  ModuleBase_Operation* anOperation = myWorkshop->module()->currentOperation();
  if (anOperation) {
    ModuleBase_IViewer* aViewer = myWorkshop->viewer();
    Handle(AIS_InteractiveContext) aContext = aViewer->AISContext();

    ModuleBase_IPropertyPanel* aPanel = anOperation->propertyPanel();
    if (aPanel) {
      ModuleBase_ModelWidget* aCurrentWidget = aPanel->preselectionWidget();
      if (!aCurrentWidget)
        aCurrentWidget = myWorkshop->module()->activeWidget();
      if (aCurrentWidget) {
        ModuleBase_ViewerPrsPtr aPrs(new ModuleBase_ViewerPrs());
        myWorkshop->selection()->fillPresentation(aPrs, theOwner);
        if (aPrs->isEmpty())
          return false;
        ModuleBase_WidgetValidated* aWidgetValidated = dynamic_cast<ModuleBase_WidgetValidated*>
                                                                               (aCurrentWidget);
        if (aWidgetValidated)
          aValid = !aWidgetValidated || aWidgetValidated->isValidSelection(aPrs);
        else{
          ModuleBase_WidgetValidator* aWidgetValidator = aCurrentWidget->widgetValidator();
          if (aWidgetValidator)
            aValid = aWidgetValidator->isValidSelection(aPrs);
        }
      }
    }
  }

#ifdef DEBUG_FILTERS
  qDebug(QString("ModuleBase_FilterValidated::IsOk = %1").arg(aValid).toStdString().c_str());
#endif
  return aValid;
}

