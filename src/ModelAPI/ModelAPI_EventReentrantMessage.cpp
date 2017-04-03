// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    ModelAPI_EventReentrantMessage.cpp
// Created: 30 Mar 2017
// Author:  Natalia ERMOLAEVA

#include <ModelAPI_EventReentrantMessage.h>

ModelAPI_EventReentrantMessage::ModelAPI_EventReentrantMessage(
                                                 const Events_ID theID,
                                                 const void* theSender)
: Events_Message(theID, theSender)
{
}
