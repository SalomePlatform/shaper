// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef GEOMALGOAPI_TOOLS_H_
#define GEOMALGOAPI_TOOLS_H_

#include <GeomAlgoAPI.h>

#include <string>

namespace GeomAlgoAPI_Tools {

/** \class Localizer
 *  \ingroup DataAlgo
 *  \brief Localizer tool.
 */
class Localizer
{
public:
  GEOMALGOAPI_EXPORT Localizer();
  GEOMALGOAPI_EXPORT ~Localizer();
private:
  std::string myCurLocale;
};

/** \class File_Tools
 *  \ingroup DataAlgo
 *  \brief File tool.
 */
class File_Tools {
public:
  /**
   * Returns an extension of theFileName
   */
  GEOMALGOAPI_EXPORT static std::string extension(const std::string& theFileName);
  /**
   * Returns a name of theFileName
   */
  GEOMALGOAPI_EXPORT static std::string name(const std::string& theFileName);
};

} // GeomAlgoAPI_Tools

#endif /* GEOMALGOAPI_TOOLS_H_ */
