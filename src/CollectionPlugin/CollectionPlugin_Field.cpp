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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "CollectionPlugin_Field.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeStringArray.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeTables.h>
#include <ModelAPI_ResultField.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

CollectionPlugin_Field::CollectionPlugin_Field()
{
}

void CollectionPlugin_Field::initAttributes()
{
  data()->addAttribute(SELECTED_ID(), ModelAPI_AttributeSelectionList::typeId());
  // for the whole part result is not obligatory
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SELECTED_ID());

  data()->addAttribute(COMPONENTS_NAMES_ID(), ModelAPI_AttributeStringArray::typeId());
  data()->addAttribute(STAMPS_ID(), ModelAPI_AttributeIntArray::typeId());
  data()->addAttribute(VALUES_ID(), ModelAPI_AttributeTables::typeId());
}

void CollectionPlugin_Field::execute()
{
  if (results().empty() || firstResult()->isDisabled()) { // just create result if not exists
    ResultPtr aField = document()->createField(data());
    setResult(aField);
  }
}
