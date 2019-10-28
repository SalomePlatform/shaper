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

#include <ExchangePlugin_ImportPart.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>

ExchangePlugin_ImportPart::ExchangePlugin_ImportPart()
{
}

void ExchangePlugin_ImportPart::initAttributes()
{
  data()->addAttribute(FILE_PATH_ID(), ModelAPI_AttributeString::typeId());
}

void ExchangePlugin_ImportPart::execute()
{
  AttributeStringPtr aFilePathAttr = string(FILE_PATH_ID());
  std::string aFilename = aFilePathAttr->value();
  if (aFilename.empty()) {
    setError("File path is empty.");
    return;
  }

  // load the file into the active document
  SessionPtr aSession = ModelAPI_Session::get();
  if (!aSession->activeDocument()->import(aFilename.c_str()))
    setError("Cannot import the document.");
}
