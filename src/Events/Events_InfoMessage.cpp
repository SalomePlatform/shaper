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

#ifdef WIN32
#pragma warning(disable : 4996) // for sprintf
#endif

void Events_InfoMessage::addParameter(double theParam)
{
  static char aBuf[50];
  sprintf(aBuf, "%g", theParam);
  myParameters.push_back(std::string(aBuf));
}

void Events_InfoMessage::addParameter(int theParam)
{
  static char aBuf[50];
  sprintf(aBuf, "%d", theParam);
  myParameters.push_back(std::string(aBuf));
}

void Events_InfoMessage::send()
{
  std::shared_ptr<Events_Message> aMsg(new Events_InfoMessage(*this));
  Events_Loop::loop()->send(aMsg);
}
