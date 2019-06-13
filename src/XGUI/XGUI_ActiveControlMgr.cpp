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

#include "XGUI_ActiveControlMgr.h"
#include "XGUI_ActiveControlSelector.h"
#include "XGUI_SelectionActivate.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Tools.h"
#include "XGUI_Workshop.h"

#include <ModuleBase_IModule.h>
#include <ModuleBase_IWorkshop.h>

//#define DEBUG_ACTIVE_SELECTOR

#ifdef DEBUG_ACTIVE_SELECTOR
void debugInfo(const QString& theMessage, XGUI_ActiveControlSelector* theSelector)
{
  std::cout << theMessage.toStdString().c_str() << ", active: "
    << (theSelector ? theSelector->getType().toStdString().c_str() : "NULL") << std::endl;
}
#endif

//********************************************************************
XGUI_ActiveControlMgr::XGUI_ActiveControlMgr(ModuleBase_IWorkshop* theWorkshop)
: myWorkshop(theWorkshop), myActiveSelector(0), myIsBlocked(false)
{
  connect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

//********************************************************************
void XGUI_ActiveControlMgr::addSelector(XGUI_ActiveControlSelector* theSelector)
{
  mySelectors.append(theSelector);
  connect(theSelector, SIGNAL(activated()), this, SLOT(onSelectorActivated()));
  connect(theSelector, SIGNAL(deactivated()), this, SLOT(onSelectorDeactivated()));
}

//********************************************************************
XGUI_ActiveControlSelector* XGUI_ActiveControlMgr::getSelector(const QString& theType)
{
  XGUI_ActiveControlSelector* aSelector;
  for (int i = 0, aCount = mySelectors.count(); i < aCount; i++)
  {
    if (mySelectors[i]->getType() != theType)
      continue;
    aSelector = mySelectors[i];
    break;
  }
  return aSelector;
}

//********************************************************************
void XGUI_ActiveControlMgr::onSelectorActivated()
{
  XGUI_ActiveControlSelector* aSelector = qobject_cast<XGUI_ActiveControlSelector*>(sender());
  if (!aSelector || aSelector == myActiveSelector)
    return;

  if (myIsBlocked) // we've come here from the same method
    return;
  myIsBlocked = true;
  if (myActiveSelector) {
    myActiveSelector->setActive(false);
  }
  activateSelector(aSelector);
  XGUI_Tools::workshop(myWorkshop)->selectionActivate()->updateSelectionModes();
  XGUI_Tools::workshop(myWorkshop)->selectionActivate()->updateSelectionFilters();

#ifdef DEBUG_ACTIVE_SELECTOR
  debugInfo("onSelectorActivated", myActiveSelector);
#endif
  myIsBlocked = false;
}

//********************************************************************
void XGUI_ActiveControlMgr::onSelectorDeactivated()
{
  XGUI_ActiveControlSelector* aSelector = qobject_cast<XGUI_ActiveControlSelector*>(sender());
  if (!aSelector || aSelector != myActiveSelector || !myActiveSelector)
    return;

  if (myIsBlocked) // we've come here from the same method
    return;
  myIsBlocked = true;

  myActiveSelector->setActive(false);
  activateSelector(NULL);

  XGUI_ActiveControlSelector* aSelectorToBeActivated = 0;
  for (int i = 0, aCount = mySelectors.count(); i < aCount; i++)
  {
    if (!mySelectors[i]->needToBeActivated())
      continue;
    aSelectorToBeActivated = mySelectors[i];
    break;
  }
  if (aSelectorToBeActivated)
    activateSelector(aSelectorToBeActivated);

  XGUI_Tools::workshop(myWorkshop)->selectionActivate()->updateSelectionModes();
  XGUI_Tools::workshop(myWorkshop)->selectionActivate()->updateSelectionFilters();
#ifdef DEBUG_ACTIVE_SELECTOR
  debugInfo("onSelectorDeactivated", myActiveSelector);
#endif
  myIsBlocked = false;
}

//********************************************************************
void XGUI_ActiveControlMgr::onSelectionChanged()
{
  if (!myActiveSelector)
    return;

  myActiveSelector->processSelection();
#ifdef DEBUG_ACTIVE_SELECTOR
  debugInfo("onSelectionChanged", myActiveSelector);
#endif
}

//********************************************************************
void XGUI_ActiveControlMgr::activateSelector(XGUI_ActiveControlSelector* theSelector)
{
  myActiveSelector = theSelector;
  if (myActiveSelector)
    myActiveSelector->setActive(true);
}
