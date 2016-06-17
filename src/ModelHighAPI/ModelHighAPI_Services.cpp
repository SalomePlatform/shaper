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
