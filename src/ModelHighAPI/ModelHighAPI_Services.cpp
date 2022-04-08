// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "ModelHighAPI_Services.h"
//--------------------------------------------------------------------------------------
#include <GeomAPI_Ax3.h>
#include <GeomAPI_Pnt.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultPart.h>

#include <cmath>
#include <sstream>

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelAPI_Document> moduleDocument()
{
  return ModelAPI_Session::get()->moduleDocument();
}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelAPI_Document> activeDocument()
{
  return ModelAPI_Session::get()->activeDocument();
}

//--------------------------------------------------------------------------------------
std::shared_ptr<GeomAPI_Ax3> defaultPlane( const std::wstring& theName )
{
  std::shared_ptr<GeomAPI_Pnt> o(new GeomAPI_Pnt(0, 0, 0));
  std::shared_ptr<GeomAPI_Dir> n, x;
  if (theName == L"XOY") {
      n.reset(new GeomAPI_Dir(0, 0, 1));
      x.reset(new GeomAPI_Dir(1, 0, 0));
  } else if (theName == L"XOZ") {
      n.reset(new GeomAPI_Dir(0, -1, 0));
      x.reset(new GeomAPI_Dir(1, 0, 0));
  } else if (theName == L"YOZ") {
      n.reset(new GeomAPI_Dir(1, 0, 0));
      x.reset(new GeomAPI_Dir(0, 1, 0));
  }

  return std::shared_ptr<GeomAPI_Ax3>(new GeomAPI_Ax3(o, x, n));
}

std::wstring defaultPlane(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                         const std::shared_ptr<GeomAPI_Dir>& theNormal,
                         const std::shared_ptr<GeomAPI_Dir>& theDirX)
{
  static const double aTol = 1.e-10;

  if (fabs(theOrigin->x()) > aTol || fabs(theOrigin->y()) > aTol || fabs(theOrigin->z()) > aTol)
    return std::wstring();

  // XOY or XOZ
  if (fabs(theNormal->x()) < aTol &&
      fabs(theDirX->x() - 1.0) < aTol && fabs(theDirX->y()) < aTol && fabs(theDirX->z()) < aTol) {
    // XOY
    if (fabs(theNormal->y()) < aTol && fabs(theNormal->z() - 1.0) < aTol)
      return std::wstring(L"XOY");
    else if (fabs(theNormal->y() + 1.0) < aTol && fabs(theNormal->z()) < aTol)
      return std::wstring(L"XOZ");
  }
  // YOZ
  else if (fabs(theNormal->x() - 1.0) < aTol &&
           fabs(theNormal->y()) < aTol && fabs(theNormal->z()) < aTol &&
           fabs(theDirX->x()) < aTol && fabs(theDirX->y() - 1.0) < aTol &&
           fabs(theDirX->z()) < aTol)
    return std::wstring(L"YOZ");

  return std::wstring();
}

std::shared_ptr<ModelAPI_Result> standardPlane(const std::wstring & theName){
  DocumentPtr aPartSet = ModelAPI_Session::get()->moduleDocument();
  // searching for the construction element
  return std::dynamic_pointer_cast<ModelAPI_Result>(
    aPartSet->objectByName(ModelAPI_ResultConstruction::group(), theName));
}

//--------------------------------------------------------------------------------------
void begin()
{
  static int aTransactionID = 0;
  static int aNbTransactions = -1;
  int aNbUndo = (int)ModelAPI_Session::get()->undoList().size();
  if (aNbUndo != aNbTransactions) {
    // the last transaction was not empty, thus increase the ID
    aNbTransactions = aNbUndo;
    ++aTransactionID;
  }
  std::ostringstream aTransactionName;
  aTransactionName << "Operation_" << aTransactionID;
	
  // check the first transaction and part, automatically created on start of PartSet
  std::shared_ptr<ModelAPI_Session> aSession = ModelAPI_Session::get();
  if (aSession->undoList().empty() && aSession->redoList().empty() && // no undo/redo available
      aSession->moduleDocument()->size(ModelAPI_ResultPart::group()) == 1 && // only one part
      aSession->moduleDocument()->size(ModelAPI_Feature::group()) == 1) // only part feature
  {
    ResultPartPtr aPartRes = std::dynamic_pointer_cast<ModelAPI_ResultPart>
      (aSession->moduleDocument()->object(ModelAPI_ResultPart::group(), 0));
    if (aPartRes.get() && aPartRes->isActivated())
    {
      DocumentPtr aPartDoc = aPartRes->partDoc();
      if (aPartDoc.get() && aPartDoc->size(ModelAPI_Feature::group()) == 0) // no features in part
      {
        // remove the automtically created part
        aSession->startOperation("Delete automatic part");
        FeaturePtr aPartFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(
          aSession->moduleDocument()->object(ModelAPI_Feature::group(), 0));
        aSession->setActiveDocument(aSession->moduleDocument());
        aSession->moduleDocument()->removeFeature(aPartFeature);
        aSession->finishOperation();
        aSession->clearUndoRedo();
      }
    }
  }

  aSession->startOperation(aTransactionName.str());
}

void end()
{
  // some operations make the current feature not the last one (like "galeries" change parameters)
  DocumentPtr anActive = ModelAPI_Session::get()->activeDocument();
  int aSize = anActive->size("Features");
  if (aSize > 0) {
    FeaturePtr aLastFeat =
      std::dynamic_pointer_cast<ModelAPI_Feature>(anActive->object("Features", aSize - 1));
    anActive->setCurrentFeature(aLastFeat, true);
  }

  ModelAPI_Session::get()->finishOperation();
  // to update data tree in the end of dumped script execution
  ModelAPI_EventCreator::get()->sendReordered(FeaturePtr());
}
void apply()
{
  auto aSession = ModelAPI_Session::get();
  aSession->finishOperation();
  // start the next operation
  begin();
}

void updateFeatures()
{
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
}

//--------------------------------------------------------------------------------------
void undo()
{
  ModelAPI_Session::get()->undo();
}
void redo()
{
  ModelAPI_Session::get()->redo();
}

//--------------------------------------------------------------------------------------
void reset()
{
  ModelAPI_Session::get()->closeAll();
}

//--------------------------------------------------------------------------------------
