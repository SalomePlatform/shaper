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

#include "Events_InfoMessage.h"
#include <sstream>

// To support old types of GCC (less than 5.0), check the wide-string conversion is working
#if (__cplusplus >= 201103L || _MSVC_LANG >= 201103L)  && \
    (__cplusplus >= 201402L || !defined(__GLIBCXX__)   || \
    (defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE > 4))
#define HAVE_WORKING_WIDESTRING 1
#else
#define HAVE_WORKING_WIDESTRING 0
#endif

#if HAVE_WORKING_WIDESTRING
#include <codecvt>
#endif

void Events_InfoMessage::addParameter(double theParam)
{
  std::stringstream aStream;
  aStream << theParam;
  myParameters.push_back(aStream.str());
}

void Events_InfoMessage::addParameter(int theParam)
{
  std::stringstream aStream;
  aStream << theParam;
  myParameters.push_back(aStream.str());
}

void Events_InfoMessage::send()
{
  std::shared_ptr<Events_Message> aMsg(new Events_InfoMessage(*this));
  Events_Loop::loop()->send(aMsg);
}

Events_InfoMessage& Events_InfoMessage::arg(const std::wstring& theParam)
{
#if HAVE_WORKING_WIDESTRING
  static std::wstring_convert<std::codecvt_utf8<wchar_t> > aConvertor;
  addParameter(aConvertor.to_bytes(theParam));
#else
  char* aBuf = new char[2 * (theParam.size() + 1)];
  size_t aNbChars = std::wcstombs(aBuf, theParam.c_str(), theParam.size());
  if (aNbChars != (size_t)-1)
    aBuf[aNbChars] = '\0';
  addParameter(aBuf);
  delete[] aBuf;
#endif
  return *this;
}
