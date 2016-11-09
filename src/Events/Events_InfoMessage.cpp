// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:	Events_InfoMessage.cpp
// Created:	16 June 2016
// Author:	Vitaly SMETANNIKOV

#include "Events_InfoMessage.h"

#ifdef WIN32
#pragma warning(disable : 4996) // for sprintf
#endif

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


std::string insertParameters(const std::string& theString, const std::list<std::string>& theParams)
{
  std::string aResult = theString;
  std::list<std::string>::const_iterator aIt;
  int i;
  char aBuf[20];
  std::string aParam;
  for (i=1, aIt = theParams.cbegin(); aIt != theParams.cend(); aIt++, i++) {
    aParam = (*aIt);
    sprintf(aBuf, "%d", i);
    std::string aCode = std::string("%") + std::string(aBuf);
    size_t aPos = aResult.find(aCode);
    if (aPos != std::string::npos) {
      std::string aFirst = aResult.substr(0, aPos);
      std::string aLast = aResult.substr(aPos + aCode.length(), std::string::npos);
      aResult = aFirst + aParam + aLast;
    }
  }
  return aResult;
}

std::string Events_InfoMessage::messageString() const 
{ 
  return insertParameters(myMessage, myParameters); 
}
