// Copyright (C) 2019-2026  CEA, EDF
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

#include "SketchPlugin_ConstraintCoincidenceInternal.h"

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

SketchPlugin_ConstraintCoincidenceInternal::SketchPlugin_ConstraintCoincidenceInternal()
{
}

void SketchPlugin_ConstraintCoincidenceInternal::initAttributes()
{
  // #1 ENTITY_A
  // #2 ENTITY_B
  AttributeRefAttrPtr anEntityAAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>
    (data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId()));
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());

  if (anEntityAAttr->isInitialized()) {
    // we are opening a HDF study:
    // #3 ?CONSTRAINT_ACTIVE?
    AttributeBooleanPtr anActiveAttr = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>
      (data()->addAttribute(SketchPlugin_Constraint::CONSTRAINT_ACTIVE(),
                            ModelAPI_AttributeBoolean::typeId())); // #3 ?
    if (!anActiveAttr->isInitialized()) {
      // Salome 9.13 or earlier (no CONSTRAINT_ACTIVE attribute at all)
      // #3 INDEX_ENTITY_A
      // #4 INDEX_ENTITY_B
      // #5 CONSTRAINT_ACTIVE
      data()->addAttribute(INDEX_ENTITY_A(), ModelAPI_AttributeInteger::typeId(), 3); // #3
      data()->addAttribute(INDEX_ENTITY_B(), ModelAPI_AttributeInteger::typeId(), 4); // #4

      anActiveAttr = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>
        (data()->addAttribute(SketchPlugin_Constraint::CONSTRAINT_ACTIVE(),
                              ModelAPI_AttributeBoolean::typeId(),
                              5)); // #5
      anActiveAttr->setValue(true);
    }
    else {
      // Salome 9.14 or newer
      // #3 CONSTRAINT_ACTIVE already initialized
      // #4 INDEX_ENTITY_A
      // #5 INDEX_ENTITY_B
      data()->addAttribute(INDEX_ENTITY_A(), ModelAPI_AttributeInteger::typeId()); // #4
      data()->addAttribute(INDEX_ENTITY_B(), ModelAPI_AttributeInteger::typeId()); // #5
    }
  }
  else {
    // we are creating constraint from scratch
    // #3 CONSTRAINT_ACTIVE set to true by default
    // #4 INDEX_ENTITY_A
    // #5 INDEX_ENTITY_B
    AttributeBooleanPtr anActiveAttr = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>
      (data()->addAttribute(SketchPlugin_Constraint::CONSTRAINT_ACTIVE(),
                            ModelAPI_AttributeBoolean::typeId())); // #3
    anActiveAttr->setValue(true);

    data()->addAttribute(INDEX_ENTITY_A(), ModelAPI_AttributeInteger::typeId()); // #4
    data()->addAttribute(INDEX_ENTITY_B(), ModelAPI_AttributeInteger::typeId()); // #5
  }

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), INDEX_ENTITY_A());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), INDEX_ENTITY_B());
}

void SketchPlugin_ConstraintCoincidenceInternal::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintCoincidenceInternal::getAISObject(AISObjectPtr thePrevious)
{
  return AISObjectPtr();
}
