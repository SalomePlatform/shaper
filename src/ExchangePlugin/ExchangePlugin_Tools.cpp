// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    ExchangePlugin_Tools.cpp
// Created: May 15, 2015
// Author:  Sergey POKHODENKO

#include <ExchangePlugin_Tools.h>

#include <sstream>

std::list<std::string> ExchangePlugin_Tools::split(const std::string& theString, char theDelimiter)
{
  std::list<std::string> theResult;
  std::istringstream aStream(theString);
  std::string aSection;
  while (std::getline(aStream, aSection, theDelimiter))
    theResult.push_back(aSection);
  return theResult;
}

std::string ExchangePlugin_Tools::selectionType2xaoDimension(const std::string& theType)
{
  if (theType == "Vertices")
    return "vertex";
  else if (theType == "Edges")
    return "edge";
  else if (theType == "Faces")
    return "face";
  else if (theType == "Solids")
    return "solid";

  return std::string();
}

std::string ExchangePlugin_Tools::xaoDimension2selectionType(const std::string& theDimension)
{
  if (theDimension == "vertex")
    return "Vertices";
  else if (theDimension == "edge")
    return "Edges";
  else if (theDimension == "face")
    return "Faces";
  else if (theDimension == "solid")
    return "Solids";

  return std::string();
}

