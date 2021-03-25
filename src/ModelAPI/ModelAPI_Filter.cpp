// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "ModelAPI_Filter.h"

#include <Config_XMLReader.h>

#include <fstream>

std::string ModelAPI_Filter::xmlFromFile(const std::string& theConfigFile) const
{
  std::string aXmlRepresentation;
  std::string aFullPath = Config_XMLReader::findConfigFile(theConfigFile);
  if (!aFullPath.empty()) {
    std::ifstream anInputFile(aFullPath);
    aXmlRepresentation.assign(std::istreambuf_iterator<char>(anInputFile),
                              std::istreambuf_iterator<char>());
  }
  return aXmlRepresentation;
}
