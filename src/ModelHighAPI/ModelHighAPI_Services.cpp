// Name   : ModelHighAPI_Services.cpp
// Purpose: 
//
// History:
// 17/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Services.h"
//--------------------------------------------------------------------------------------
#include <GeomAPI_Ax3.h>
#include <GeomAPI_Pnt.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultConstruction.h>

#include <cmath>

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
std::shared_ptr<GeomAPI_Ax3> defaultPlane( const std::string& theName )
{
  std::shared_ptr<GeomAPI_Pnt> o(new GeomAPI_Pnt(0, 0, 0));
  std::shared_ptr<GeomAPI_Dir> n, x;
  if (theName == "XOY") {
      n.reset(new GeomAPI_Dir(0, 0, 1));
      x.reset(new GeomAPI_Dir(1, 0, 0));
  } else if (theName == "XOZ") {
      n.reset(new GeomAPI_Dir(0, -1, 0));
      x.reset(new GeomAPI_Dir(1, 0, 0));
  } else if (theName == "YOZ") {
      n.reset(new GeomAPI_Dir(1, 0, 0));
      x.reset(new GeomAPI_Dir(0, 1, 0));
  }

  return std::shared_ptr<GeomAPI_Ax3>(new GeomAPI_Ax3(o, x, n));
}

std::string defaultPlane(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                         const std::shared_ptr<GeomAPI_Dir>& theNormal,
                         const std::shared_ptr<GeomAPI_Dir>& theDirX)
{
  static const double aTol = 1.e-10;

  if (fabs(theOrigin->x()) > aTol || fabs(theOrigin->y()) > aTol || fabs(theOrigin->z()) > aTol)
    return std::string();

  // XOY or XOZ
  if (fabs(theNormal->x()) < aTol && 
      fabs(theDirX->x() - 1.0) < aTol && fabs(theDirX->y()) < aTol && fabs(theDirX->z()) < aTol) {
    // XOY
    if (fabs(theNormal->y()) < aTol && fabs(theNormal->z() - 1.0) < aTol)
      return std::string("XOY");
    else if (fabs(theNormal->y() + 1.0) < aTol && fabs(theNormal->z()) < aTol)
      return std::string("XOZ");
  }
  // YOZ
  else if (fabs(theNormal->x() - 1.0) < aTol && fabs(theNormal->y()) < aTol && fabs(theNormal->z()) < aTol &&
           fabs(theDirX->x()) < aTol && fabs(theDirX->y() - 1.0) < aTol && fabs(theDirX->z()) < aTol)
    return std::string("YOZ");

  return std::string();
}

std::shared_ptr<ModelAPI_Result> standardPlane(const std::string & theName){
  DocumentPtr aPartSet = ModelAPI_Session::get()->moduleDocument();
  // searching for the construction element
  return std::dynamic_pointer_cast<ModelAPI_Result>(
    aPartSet->objectByName(ModelAPI_ResultConstruction::group(), theName));
}

//--------------------------------------------------------------------------------------
void begin()
{
  ModelAPI_Session::get()->startOperation();
}
void end()
{
  ModelAPI_Session::get()->finishOperation();
}
void apply()
{
  auto aSession = ModelAPI_Session::get();
  aSession->finishOperation();
  aSession->startOperation();
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
