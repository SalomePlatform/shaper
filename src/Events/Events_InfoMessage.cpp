// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:	Events_InfoMessage.cpp
// Created:	16 June 2016
// Author:	Vitaly SMETANNIKOV

#include "Events_InfoMessage.h"


void Events_InfoMessage::addParameter(double theParam) 
{ 
  char aBuf[50];
  int n = sprintf(aBuf, "%g", theParam);
  std::string aStr(aBuf);
  myParameters.push_back(aStr); 
}

void Events_InfoMessage::addParameter(int theParam) 
{ 
  char aBuf[50];
  int n = sprintf(aBuf, "%d", theParam);
  std::string aStr(aBuf);
  myParameters.push_back(aStr); 
}

void Events_InfoMessage::send() 
{ 
  std::shared_ptr<Events_Message> aMsg(new Events_InfoMessage(*this));
  Events_Loop::loop()->send(aMsg); 
}
