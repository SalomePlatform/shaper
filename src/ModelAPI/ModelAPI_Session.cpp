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

#include <ModelAPI_Session.h>
// to avoid unresolved ModelAPI_Document()
#include <ModelAPI_Document.h>
// to avoid unresolved ModelAPI_Feature()
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
// to avoid unresolved ModelAPI_Data()
#include <ModelAPI_Data.h>
// to avoid unresolved ModelAPI_Plugin()
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <Config_ModuleReader.h>
#include <Config_PropManager.h>

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#ifdef _DEBUG
#include <iostream>
#endif

/// Manager that will be initialized from Model package, one per application
std::shared_ptr<ModelAPI_Session> MY_MANAGER;

void ModelAPI_Session::setSession(std::shared_ptr<ModelAPI_Session> theManager)
{
  MY_MANAGER = theManager;
}

std::shared_ptr<ModelAPI_Session> ModelAPI_Session::get()
{
  if (!MY_MANAGER) {  // import Model library that implements this interface of ModelAPI
    Config_ModuleReader::loadLibrary("Model");

    Config_PropManager::registerProp("Visualization", "body_deflection",
                                     "Body deflection coefficient",
                                     Config_Prop::Double,
                                     ModelAPI_ResultBody::DEFAULT_DEFLECTION());

    Config_PropManager::registerProp("Visualization", "construction_deflection",
                                     "Construction deflection coefficient",
                                     Config_Prop::Double,
                                     ModelAPI_ResultConstruction::DEFAULT_DEFLECTION());

    Config_PropManager::registerProp("Visualization", "default_transparency",
      "Default transparency", Config_Prop::Double, "0");

  }
  return MY_MANAGER;
}

/// instance of the events creator, one pre application
const ModelAPI_EventCreator* MY_API_CREATOR = 0;

const ModelAPI_EventCreator* ModelAPI_EventCreator::get()
{
  if (!MY_API_CREATOR) {  // import Model library that implements this interface of ModelAPI
    Config_ModuleReader::loadLibrary("Model");
  }
  return MY_API_CREATOR;
}

void ModelAPI_EventCreator::set(const ModelAPI_EventCreator* theCreator)
{
  MY_API_CREATOR = theCreator;
}
