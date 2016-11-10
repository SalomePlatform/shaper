// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// Name   : ConnectorAPI_Connector.h
// Purpose:
//
// History:
// 16/06/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_CONNECTORAPI_CONNECTORAPI_PART_H_
#define SRC_CONNECTORAPI_CONNECTORAPI_PART_H_

//--------------------------------------------------------------------------------------
#include "ConnectorAPI.h"
//--------------------------------------------------------------------------------------
#include <memory>
//--------------------------------------------------------------------------------------
class ModelAPI_Document;
//--------------------------------------------------------------------------------------
/**\ingroup CPPHighAPI
 * \brief Export to GEOM
 */
CONNECTORAPI_EXPORT
void exportToGEOM(const std::shared_ptr<ModelAPI_Document> & thePart);

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_CONNECTORAPI_CONNECTORAPI_PART_H_ */
