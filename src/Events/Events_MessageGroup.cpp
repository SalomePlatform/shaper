// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:	Events_MessageGroup.cpp
// Created:	Thu Mar 13 2014
// Author:	Mikhail PONIKAROV

#include <Events_MessageGroup.h>

Events_MessageGroup::Events_MessageGroup(const Events_ID theID, const void* theSender)
: Events_Message(theID, theSender)
{}

Events_MessageGroup::~Events_MessageGroup()
{}

