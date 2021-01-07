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

#ifndef GeomAlgoAPI_Exception_H_
#define GeomAlgoAPI_Exception_H_

#include <GeomAlgoAPI.h>

#include <iostream>

/**\class GeomAlgoAPI_Exception
 * \ingroup DataAlgo
 * \brief Manage exceptions
 */
class GeomAlgoAPI_Exception : public std::exception
{
 public:
  /// Create an exception
  /// \param theMessageError Error message to be displayed
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Exception(std::string theMessageError);
  /// Destroyer
  GEOMALGOAPI_EXPORT ~GeomAlgoAPI_Exception() noexcept;
  /// Allows to collet the error
  GEOMALGOAPI_EXPORT const char* what() const noexcept;

private:
  std::string myMessageError; /// Error message to be displayed.
};


#endif
