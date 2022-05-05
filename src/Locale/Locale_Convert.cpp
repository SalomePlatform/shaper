// Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

#include <Locale_Convert.h>

#include <boost/locale/encoding_utf.hpp>

namespace Locale
{
  std::string Convert::toString(const std::wstring& theWStr)
  {
    return boost::locale::conv::utf_to_utf<char>(theWStr);
  }

  std::string Convert::toString(const char16_t* theExtStr)
  {
    return boost::locale::conv::utf_to_utf<char>(theExtStr);
  }

  std::wstring Convert::toWString(const std::string& theStr)
  {
    return boost::locale::conv::utf_to_utf<wchar_t>(theStr);
  }

  std::wstring Convert::toWString(const char16_t* theExtStr)
  {
    return boost::locale::conv::utf_to_utf<wchar_t>(theExtStr);
  }

}
