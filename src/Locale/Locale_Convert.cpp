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

#include <Locale_Convert.h>

#include <boost/locale/encoding_utf.hpp>

////// To support old types of GCC (less than 5.0), check the wide-string conversion is working
////#if (__cplusplus >= 201103L || _MSVC_LANG >= 201103L)  && \
////    (__cplusplus >= 201402L || !defined(__GLIBCXX__)   || \
////    (defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE > 4))
////#define HAVE_WORKING_WIDESTRING 1
////#else
////#define HAVE_WORKING_WIDESTRING 0
////#endif
////
////#if HAVE_WORKING_WIDESTRING
////#include <codecvt>
////#endif

namespace Locale
{
  std::string Convert::toString(const std::wstring& theWStr)
  {
////#if HAVE_WORKING_WIDESTRING
////    static std::wstring_convert<std::codecvt_utf8<wchar_t> > aConvertor;
////    return aConvertor.to_bytes(theWStr);
////#else
////    char* aBuf = new char[2 * (theWStr.size() + 1)];
////    size_t aNbChars = std::wcstombs(aBuf, theWStr.c_str(), theWStr.size());
////    if (aNbChars != (size_t)-1)
////      aBuf[aNbChars] = '\0';
////    std::string aStr(aBuf);
////    delete[] aBuf;
////    return aStr;
////#endif
    return boost::locale::conv::utf_to_utf<char>(theWStr);
  }

  std::string Convert::toString(const char16_t* theExtStr)
  {
    return boost::locale::conv::utf_to_utf<char>(theExtStr);
  }

  std::wstring Convert::toWString(const std::string& theStr)
  {
////#if HAVE_WORKING_WIDESTRING
////    static std::wstring_convert<std::codecvt_utf8<wchar_t> > aConvertor;
////    return aConvertor.from_bytes(theStr);
////#else
////    wchar_t* aBuf = new wchar_t[theStr.size() + 1];
////    size_t aNbWChars = std::mbstowcs(aBuf, theStr.c_str(), theStr.size());
////    if (aNbWChars != (size_t)-1)
////      aBuf[aNbWChars] = L'\0';
////    std::wstring aWStr(aBuf);
////    delete[] aBuf;
////    return aWStr;
////#endif
    return boost::locale::conv::utf_to_utf<wchar_t>(theStr);
  }

  std::wstring Convert::toWString(const char16_t* theExtStr)
  {
    return boost::locale::conv::utf_to_utf<wchar_t>(theExtStr);
  }

}
