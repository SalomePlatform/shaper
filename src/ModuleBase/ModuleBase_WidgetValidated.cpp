// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <ModuleBase_WidgetValidated.h>
#include <ModuleBase_FilterFactory.h>
#include <ModuleBase_IViewer.h>
#include <SelectMgr_ListIteratorOfListOfFilter.hxx>

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_ResultValidator.h>

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

//********************************************************************
bool ModuleBase_WidgetValidated::isValid(ObjectPtr theObj, GeomShapePtr theShape) const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  std::list<ModelAPI_Validator*> aValidators;
  std::list<std::list<std::string> > anArguments;
  aFactory->validators(parentID(), attributeID(), aValidators, anArguments);

  // Check the type of selected object
  std::list<ModelAPI_Validator*>::iterator aValidator = aValidators.begin();
  bool isValid = true;
  for (; aValidator != aValidators.end(); aValidator++) {
    const ModelAPI_ResultValidator* aResValidator =
        dynamic_cast<const ModelAPI_ResultValidator*>(*aValidator);
    if (aResValidator) {
      isValid = false;
      if (aResValidator->isValid(theObj)) {
        isValid = true;
        break;
      }
    }
  }
  return isValid;
}

void ModuleBase_WidgetValidated::activateFilters(ModuleBase_IWorkshop* theWorkshop,
                                                 const bool toActivate) const
{
  ModuleBase_IViewer* aViewer = theWorkshop->viewer();

  // apply filters loaded from the XML definition of the widget
  ModuleBase_FilterFactory* aFactory = theWorkshop->selectionFilters();
  SelectMgr_ListOfFilter aFactoryFilters;
  aFactory->filters(parentID(), attributeID(), aFactoryFilters);
  SelectMgr_ListIteratorOfListOfFilter aFactoryIt(aFactoryFilters);
  for (; aFactoryIt.More(); aFactoryIt.Next()) {
    Handle(SelectMgr_Filter) aSelFilter = aFactoryIt.Value();
    if (aSelFilter.IsNull())
      continue;
    if (toActivate)
      aViewer->addSelectionFilter(aSelFilter);
    else
      aViewer->removeSelectionFilter(aSelFilter);
  }
}

void ModuleBase_WidgetValidated::selectionFilters(ModuleBase_IWorkshop* theWorkshop,
                                                  SelectMgr_ListOfFilter& theFilters) const
{
  ModuleBase_FilterFactory* aFactory = theWorkshop->selectionFilters();
  SelectMgr_ListOfFilter aFilters;
  aFactory->filters(parentID(), attributeID(), aFilters);
  SelectMgr_ListIteratorOfListOfFilter aIt(aFilters);
  for (; aIt.More(); aIt.Next()) {
    Handle(SelectMgr_Filter) aSelFilter = aIt.Value();
    if (aSelFilter.IsNull())
      continue;

    theFilters.Append(aSelFilter);
  }
}
