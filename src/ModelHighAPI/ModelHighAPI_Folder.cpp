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

#include "ModelHighAPI_Folder.h"
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Reference.h>
#include <ModelHighAPI_Tools.h>

#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>

//--------------------------------------------------------------------------------------

ModelHighAPI_Folder::ModelHighAPI_Folder(const std::shared_ptr<ModelAPI_Folder> & theFolder)
  : ModelHighAPI_Interface(FeaturePtr()),
    myFolder(theFolder)
{
  initialize();
}

ModelHighAPI_Folder::~ModelHighAPI_Folder()
{
}

bool ModelHighAPI_Folder::initialize()
{
  if (!myFolder) {
    throwException(ID() + " exception: The folder is NULL.");
    return false;
  }

  myFirstFeature = myFolder->reference(ModelAPI_Folder::FIRST_FEATURE_ID());
  if (!myFirstFeature)
    return false;
  myAttrGetter[ModelAPI_Folder::FIRST_FEATURE_ID()] = "firstFeature";

  myLastFeature = myFolder->reference(ModelAPI_Folder::LAST_FEATURE_ID());
  if (!myLastFeature)
    return false;
  myAttrGetter[ModelAPI_Folder::LAST_FEATURE_ID()] = "lastFeature";

  return true;
}

void ModelHighAPI_Folder::setName(const std::wstring& theName)
{
  if (myFolder && myFolder->data() && myFolder->data()->isValid())
    myFolder->data()->setName(theName);
}

std::wstring ModelHighAPI_Folder::name() const
{
  return myFolder->data()->name();
}

void ModelHighAPI_Folder::dump(ModelHighAPI_Dumper& theDumper) const
{
  const std::string& aDocName = theDumper.name(myFolder->document());

  AttributeReferencePtr aStartRef = firstFeature();
  AttributeReferencePtr aEndRef   = lastFeature();

  // do not dump empty folders
  if (!aEndRef->value())
    return;

  // Dump folder when its features have been already dumped.
  // Otherwise, store the folder postponed.
  if (theDumper.isDumped(EntityPtr(aEndRef->value())))
    theDumper << myFolder << " = model.addFolder(" << aDocName << ", "
              << aStartRef << ", " << aEndRef << ")" << std::endl;
  else
    theDumper.postpone(myFolder);
}

//--------------------------------------------------------------------------------------

std::shared_ptr<ModelHighAPI_Folder> addFolder(const std::shared_ptr<ModelAPI_Document>& theDoc)
{
  std::shared_ptr<ModelAPI_Folder> aFolder = theDoc->addFolder();
  return std::shared_ptr<ModelHighAPI_Folder>(new ModelHighAPI_Folder(aFolder));
}

std::shared_ptr<ModelHighAPI_Folder> addFolder(const std::shared_ptr<ModelAPI_Document>& theDoc,
                                               const ModelHighAPI_Reference& theFirstFeature,
                                               const ModelHighAPI_Reference& theLastFeature)
{
  std::shared_ptr<ModelAPI_Folder> aFolder = theDoc->addFolder(theFirstFeature.feature());

  AttributeReferencePtr aFirstFeatAttr = aFolder->reference(ModelAPI_Folder::FIRST_FEATURE_ID());
  fillAttribute(theFirstFeature.feature(), aFirstFeatAttr);

  AttributeReferencePtr aLastFeatAttr = aFolder->reference(ModelAPI_Folder::LAST_FEATURE_ID());
  fillAttribute(theLastFeature.feature(), aLastFeatAttr);

  // to update the folder state in the Object Browser
  theDoc->updateHistory(ModelAPI_Feature::group());

  return std::shared_ptr<ModelHighAPI_Folder>(new ModelHighAPI_Folder(aFolder));
}

void removeFolder(std::shared_ptr<ModelHighAPI_Folder>& theFolder)
{
  theFolder->folder()->document()->removeFolder(theFolder->folder());
}
