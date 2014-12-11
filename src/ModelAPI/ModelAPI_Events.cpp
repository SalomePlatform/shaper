// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModelAPI_Events.cpp
 *
 *  Created on: Dec 8, 2014
 *      Author: sbh
 */

#include <ModelAPI.h>
#include <ModelAPI_Events.h>

ModelAPI_ObjectUpdatedMessage::ModelAPI_ObjectUpdatedMessage(const Events_ID theID,
                                                             const void* theSender)
    : Events_MessageGroup(theID, theSender)
{

}

ModelAPI_ObjectUpdatedMessage::~ModelAPI_ObjectUpdatedMessage()
{

}

ModelAPI_ObjectDeletedMessage::ModelAPI_ObjectDeletedMessage(const Events_ID theID,
                                                             const void* theSender)
    : Events_MessageGroup(theID, theSender)
{

}

ModelAPI_ObjectDeletedMessage::~ModelAPI_ObjectDeletedMessage()
{

}

