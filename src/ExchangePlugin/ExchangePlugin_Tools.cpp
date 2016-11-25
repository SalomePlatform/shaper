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
  if (theType == "Vertices" || theType == "vertex")
    return "vertex";
  else if (theType == "Edges" || theType == "edge")
    return "edge";
  else if (theType == "Faces" || theType == "face")
    return "face";
  else if (theType == "Solids" || theType == "solid")
    return "solid";
  else if (theType == "Part" || theType == "part")
    return "part";

  return std::string();
}

std::string ExchangePlugin_Tools::xaoDimension2selectionType(const std::string& theDimension)
{
  if (theDimension == "vertex")
    return "vertex";
  else if (theDimension == "edge")
    return "edge";
  else if (theDimension == "face")
    return "face";
  else if (theDimension == "solid")
    return "solid";

  return std::string();
}


std::string ExchangePlugin_Tools::valuesType2xaoType(
  const ModelAPI_AttributeTables::ValueType& theType)
{
  switch(theType) {
  case ModelAPI_AttributeTables::BOOLEAN:
    return "boolean";
  case ModelAPI_AttributeTables::INTEGER:
    return "integer";
  case ModelAPI_AttributeTables::DOUBLE:
    return "double";
  case ModelAPI_AttributeTables::STRING:
    return "string";
  }
  return "";
}

ModelAPI_AttributeTables::ValueType ExchangePlugin_Tools::xaoType2valuesType(std::string theType)
{
  if (theType == "boolean")
    return ModelAPI_AttributeTables::BOOLEAN;
  if (theType == "integer")
    return ModelAPI_AttributeTables::INTEGER;
  if (theType == "double")
    return ModelAPI_AttributeTables::DOUBLE;
  if (theType == "string")
    return ModelAPI_AttributeTables::STRING;
  return ModelAPI_AttributeTables::DOUBLE;
}
