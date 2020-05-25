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

#include <SketchPlugin_Offset.h>

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>


SketchPlugin_Offset::SketchPlugin_Offset()
  : SketchPlugin_SketchEntity()
{
}

void SketchPlugin_Offset::initDerivedClassAttributes()
{
  data()->addAttribute(EDGES_ID(), ModelAPI_AttributeRefList::typeId());
  data()->addAttribute(VALUE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(REVERSED_ID(), ModelAPI_AttributeBoolean::typeId());
}

void SketchPlugin_Offset::execute()
{
}

void SketchPlugin_Offset::attributeChanged(const std::string& theID)
{
}

bool SketchPlugin_Offset::customAction(const std::string& theActionId)
{
  bool isOk = false;
  if (theActionId == ADD_WIRE_ACTION_ID()) {
    isOk = findWires();
  }
  else {
    std::string aMsg = "Error: Feature \"%1\" does not support action \"%2\".";
    Events_InfoMessage("SketchPlugin_Offset", aMsg).arg(getKind()).arg(theActionId).send();
  }
  return isOk;
}

bool SketchPlugin_Offset::findWires()
{
  return false;
}
