// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
  if (theType == "Vertices" || theType == "vertex" || theType == "VERTEX")
    return "vertex";
  else if (theType == "Edges" || theType == "edge" || theType == "EDGE")
    return "edge";
  else if (theType == "Faces" || theType == "face" || theType == "FACE")
    return "face";
  else if (theType == "Solids" || theType == "solid" || theType == "SOLID")
    return "solid";
  else if (theType == "Part" || theType == "part" || theType == "PART")
    return "whole";

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
