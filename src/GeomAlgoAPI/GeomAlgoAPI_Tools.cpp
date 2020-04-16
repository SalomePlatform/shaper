// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_Tools.h"
#include "GeomAlgoAPI_MakeShape.h"

#include <clocale>

#include <TCollection_AsciiString.hxx>
#include <OSD_Path.hxx>

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


std::string File_Tools::extension(const std::string& theFileName)
{
  // retrieve the file and plugin library names
  TCollection_AsciiString aFileName(theFileName.c_str());
  OSD_Path aPath(aFileName);
  TCollection_AsciiString anExtension = aPath.Extension();
  if (anExtension.Length() < 2)
    return "";
  // TCollection_AsciiString are numbered from 1
  anExtension = anExtension.SubString(2, anExtension.Length());
  anExtension.UpperCase();
  return anExtension.ToCString();
}

std::string File_Tools::name(const std::string& theFileName)
{
  // retrieve the file and plugin library names
  TCollection_AsciiString aFileName(theFileName.c_str());
  OSD_Path aPath(aFileName);
  return aPath.Name().ToCString();
}

bool AlgoError::isAlgorithmFailed(const GeomMakeShapePtr& theAlgorithm,
                                  const std::string& theFeature,
                                  std::string& theError)
{
  theError.clear();
  if (!theAlgorithm->isDone()) {
    theError = "Error: " + (theFeature.empty() ? "The" : theFeature) + " algorithm failed.";
    std::string anAlgoError = theAlgorithm->getError();
    if (!anAlgoError.empty())
      theError += " " + anAlgoError;
    return true;
  }
  if (!theAlgorithm->shape() || theAlgorithm->shape()->isNull()) {
    theError = "Error: Resulting shape";
    if (!theFeature.empty())
      theError += "of " + theFeature;
    theError += " is Null.";
    return true;
  }
  if (!theAlgorithm->isValid()) {
    theError = "Error: Resulting shape";
    if (!theFeature.empty())
      theError += "of " + theFeature;
    theError += " is not valid.";
    return true;
  }
  return false;
}
