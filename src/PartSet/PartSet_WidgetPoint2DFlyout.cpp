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

#include "PartSet_WidgetPoint2DFlyout.h"

#include "ModuleBase_WidgetValidator.h"
#include "ModuleBase_ViewerPrs.h"

#include <XGUI_Workshop.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_Displayer.h>

#include <Config_WidgetAPI.h>
#include <Config_Keywords.h>

#include <QWidget>


PartSet_WidgetPoint2DFlyout::PartSet_WidgetPoint2DFlyout(QWidget* theParent,
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData)
 : PartSet_WidgetPoint2D(theParent, theWorkshop, theData)
{
  myWidgetValidator = new ModuleBase_WidgetValidator(this, myWorkshop);
}

bool PartSet_WidgetPoint2DFlyout::setSelection(QList<ModuleBase_ViewerPrsPtr>& theValues,
                                               const bool theToValidate)
{
  PartSet_WidgetPoint2D::setSelection(theValues, theToValidate);
  // true value should be returned to set this control as processed the selection
  // to move focus to the next control if it is filled by preselection
  return true;
}

bool PartSet_WidgetPoint2DFlyout::isValidSelectionCustom(
                                      const std::shared_ptr<ModuleBase_ViewerPrs>& theValue)
{
  return false;
}

bool PartSet_WidgetPoint2DFlyout::useSelectedShapes() const
{
  return false;
}

bool PartSet_WidgetPoint2DFlyout::focusTo()
{
  bool aCanAcceptFocus = true;
  if (isInternal() && isComputedDefault()) {
    AISObjectPtr anObject = workshop()->displayer()->getAISObject(feature());
    aCanAcceptFocus = anObject.get() && !anObject->isEmptyDistanceGeometry();
  }
  if (aCanAcceptFocus)
    aCanAcceptFocus = PartSet_WidgetPoint2D::focusTo();
  return aCanAcceptFocus;
}

XGUI_Workshop* PartSet_WidgetPoint2DFlyout::workshop() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  return aConnector->workshop();
}
