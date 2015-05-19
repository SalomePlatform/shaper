/*
 * GeomAlgoAPI_Tools.cpp
 *
 *  Created on: May 18, 2015
 *      Author: spo
 */

#include "GeomAlgoAPI_Tools.h"

#include <clocale>

using namespace GeomAlgoAPI_Tools;

Localizer::Localizer()
{
  myCurLocale = std::setlocale(LC_NUMERIC, 0);
  std::setlocale(LC_NUMERIC, "C");
}

Localizer::~Localizer()
{
  std::setlocale(LC_NUMERIC, myCurLocale.c_str());
}
