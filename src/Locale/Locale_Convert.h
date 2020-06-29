// Copyright (C) 2020  CEA/DEN, EDF R&D
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

#ifndef LOCALE_CONVERT_H
#define LOCALE_CONVERT_H

#include <Locale_def.h>

#include <string>

namespace Locale
{
  /// Convert strings
  namespace Convert
  {
    /// Converts a wide-string to a simple (byte) string
    /// \param theWStr a wide-string
    LOCALE_EXPORT std::string toString(const std::wstring& theWStr);

    /// Converts an extended string to a simple (byte) string
    /// \param theExtStr an extended string
    LOCALE_EXPORT std::string toString(const char16_t* theExtStr);

    /// Converts a byte string to a wide-string
    /// \param theStr a byte string
    LOCALE_EXPORT std::wstring toWString(const std::string& theStr);

    /// Converts an extended string to a wide-string
    /// \param theStr a byte string
    LOCALE_EXPORT std::wstring toWString(const char16_t* theExtStr);

  }
}

#endif // Locale_Convert_H
