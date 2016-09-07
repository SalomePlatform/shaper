// Name   : ConnectorAPI_Connector.cpp
// Purpose: 
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ConnectorAPI_Connector.h"
//--------------------------------------------------------------------------------------
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
//--------------------------------------------------------------------------------------
void exportToGEOM(const std::shared_ptr<ModelAPI_Document> & thePart)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature("ExportToGEOM");
}
