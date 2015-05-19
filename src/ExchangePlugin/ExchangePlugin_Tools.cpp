/*
 * ExchangePlugin_Tools.cpp
 *
 *  Created on: May 15, 2015
 *      Author: spo
 */

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
