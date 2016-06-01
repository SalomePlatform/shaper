// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:	Events_InfoMessage.hxx
// Created:	31 May 2016
// Author:	Vitaly SMETANNIKOV

#ifndef Events_InfoMessage_H_
#define Events_InfoMessage_H_

#include <Events.h>
#include <Events_Message.h>
#include <Events_Loop.h>

class EVENTS_EXPORT Events_InfoMessage: public Events_Message
{
public:
  Events_InfoMessage(const void* theSender = 0) : 
      Events_Message(Events_Loop::eventByName("InfoMessage"), theSender) {}

  void setContext(const std::string& theContext) { myContext = theContext; } 

  std::string context() const { return myContext; }

  void setMessage(const std::string& theMsg) { myMessage = theMsg; } 

  std::string message() const { return myMessage; }

  void addParameter(const std::string& theParam) 
  { 
    myParameters.push_back(theParam); 
  }

  void addParameter(double theParam) 
  { 
    char aBuf[50];
    int n = sprintf(aBuf, "%g", theParam);
    std::string aStr(aBuf);
    myParameters.push_back(aStr); 
  }

  void addParameter(int theParam) 
  { 
    char aBuf[50];
    int n = sprintf(aBuf, "%d", theParam);
    std::string aStr(aBuf);
    myParameters.push_back(aStr); 
  }

  std::list<std::string> parameters() const { return myParameters; }

private:

  /// Context of the messgae
  std::string myContext;

  /// String of the message
  std::string myMessage;

  /// Parameters of the message
  std::list<std::string> myParameters;
};

#endif