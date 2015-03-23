// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <ModuleBase_WidgetValidated.h>
#include <ModuleBase_FilterFactory.h>
#include <ModuleBase_IViewer.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeValidator.h>

#include <SelectMgr_ListIteratorOfListOfFilter.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include <QWidget>

ModuleBase_WidgetValidated::ModuleBase_WidgetValidated(QWidget* theParent,
                                                       const Config_WidgetAPI* theData,
                                                       const std::string& theParentId)
 : ModuleBase_ModelWidget(theParent, theData, theParentId)
{
}

ModuleBase_WidgetValidated::~ModuleBase_WidgetValidated()
{
}

bool ModuleBase_WidgetValidated::setSelection(ModuleBase_ViewerPrs theValue)
{
  bool isDone = false;

  Handle(SelectMgr_EntityOwner) anOwner = theValue.owner();
  if (isValid(anOwner)) {
    setSelection(anOwner);
    updateObject(myFeature);
    emit valuesChanged();
  }
  return isDone;
}

bool ModuleBase_WidgetValidated::isValid(const Handle_SelectMgr_EntityOwner& theOwner)
{
  backupAttributeValue(true);

  setSelection(theOwner);
  bool aValid = isValidAttribute();

  backupAttributeValue(false);

  return aValid;
}

//********************************************************************
bool ModuleBase_WidgetValidated::isValidAttribute() const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  std::list<ModelAPI_Validator*> aValidators;
  std::list<std::list<std::string> > anArguments;
  aFactory->validators(myFeature->getKind(), attributeID(), aValidators, anArguments);

  DataPtr aData = myFeature->data();
  AttributePtr anAttribute = myFeature->attribute(attributeID());

  aData->blockSendAttributeUpdated(true);

    // 3. check the acceptability of the current values
  std::list<ModelAPI_Validator*>::iterator aValidator = aValidators.begin();
  std::list<std::list<std::string> >::iterator aArgs = anArguments.begin();
  bool aValid = true;
  for (; aValidator != aValidators.end() && aValid; aValidator++, aArgs++) {
    const ModelAPI_AttributeValidator* aAttrValidator =
        dynamic_cast<const ModelAPI_AttributeValidator*>(*aValidator);
    if (aAttrValidator) {
      aValid = aAttrValidator->isValid(anAttribute, *aArgs);
    }
  }
  aData->blockSendAttributeUpdated(false);

  return aValid;
}

void ModuleBase_WidgetValidated::activateFilters(ModuleBase_IWorkshop* theWorkshop,
                                                 const bool toActivate) const
{
  ModuleBase_IViewer* aViewer = theWorkshop->viewer();

  Handle(SelectMgr_Filter) aSelFilter = theWorkshop->validatorFilter();
  if (toActivate)
    aViewer->addSelectionFilter(aSelFilter);
  else
    aViewer->removeSelectionFilter(aSelFilter);
}
