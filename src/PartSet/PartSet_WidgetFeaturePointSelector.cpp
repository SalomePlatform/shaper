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

#include <Config_WidgetAPI.h>

#include <Events_Loop.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <ModuleBase_ISelection.h>
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Tools.h>

#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IModule.h>

#include "PartSet_WidgetFeaturePointSelector.h"
#include "PartSet_Tools.h"

#include <SketchPlugin_Point.h>

#include <XGUI_Tools.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>
#include <XGUI_ViewerProxy.h>

#include <QWidget>
#include <QMouseEvent>

#define HIGHLIGHT_STAYS_PROBLEM
#ifdef HIGHLIGHT_STAYS_PROBLEM
#include <Quantity_Color.hxx>
#define SKETCH_ENTITY_COLOR "225,0,0"
#endif

PartSet_WidgetFeaturePointSelector::PartSet_WidgetFeaturePointSelector(QWidget* theParent,
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData)
: ModuleBase_WidgetShapeSelector(theParent, theWorkshop, theData)
{
  std::string anAttributes = theData->getProperty("selection_attributes");
  QStringList anAttributesList = QString(anAttributes.c_str()).split(' ', QString::SkipEmptyParts);

  mySelectedObjectAttribute = anAttributesList[0].toStdString();
  mySelectedPointAttribute = anAttributesList[1].toStdString();
  myPreviewObjectAttribute = anAttributesList[2].toStdString();
  myPreviewPointAttribute = anAttributesList[3].toStdString();
}

PartSet_WidgetFeaturePointSelector::~PartSet_WidgetFeaturePointSelector()
{
}

//********************************************************************
bool PartSet_WidgetFeaturePointSelector::isValidSelection(
                                        const std::shared_ptr<ModuleBase_ViewerPrs>& theValue)
{
  return ModuleBase_WidgetShapeSelector::isValidSelection(theValue);
  //return true;
}

//********************************************************************
void PartSet_WidgetFeaturePointSelector::updateSelectionModesAndFilters(bool toActivate)
{
#ifdef HIGHLIGHT_STAYS_PROBLEM
  Handle(AIS_InteractiveContext) aContext =
                            XGUI_Tools::workshop(myWorkshop)->viewer()->AISContext();
  Quantity_Color aColor;
  Handle(Prs3d_Drawer) aHStyle = aContext->HighlightStyle();
  Handle(Prs3d_Drawer) aSStyle = aContext->SelectionStyle();
  if (toActivate) {
    std::vector<int> aColors;
    aColors = Config_PropManager::color("Visualization", "sketch_entity_color");
    aColor = Quantity_Color(aColors[0] / 255., aColors[1] / 255., aColors[2] / 255.,
                            Quantity_TOC_RGB);
    myHighlightColor = aHStyle->Color();
    mySelectionColor = aSStyle->Color();
  }
  else {
    aColor = myHighlightColor;
  }
  aHStyle->SetColor(aColor);
  aContext->SetHighlightStyle(aHStyle);

  aSStyle->SetColor(aColor);
  aContext->SetSelectionStyle(aSStyle);

#endif

  ModuleBase_WidgetShapeSelector::updateSelectionModesAndFilters(toActivate);
}

//********************************************************************
void PartSet_WidgetFeaturePointSelector::activateCustom()
{
  ModuleBase_WidgetShapeSelector::activateCustom();

  myWorkshop->module()->activateCustomPrs(myFeature,
                            ModuleBase_IModule::CustomizeHighlightedObjects, true);
}

//********************************************************************
void PartSet_WidgetFeaturePointSelector::deactivate()
{
  ModuleBase_WidgetShapeSelector::deactivate();
}

//********************************************************************
void PartSet_WidgetFeaturePointSelector::mouseMoved(ModuleBase_IViewWindow* theWindow,
                                                    QMouseEvent* theEvent)
{
  ModuleBase_ISelection* aSelect = myWorkshop->selection();
  QList<ModuleBase_ViewerPrsPtr> aHighlighted = aSelect->getHighlighted();

  ModuleBase_ViewerPrsPtr aPrs = !aHighlighted.empty() ? aHighlighted.first()
                                                       : ModuleBase_ViewerPrsPtr();
  fillFeature(aPrs, theWindow, theEvent);
}

//********************************************************************
void PartSet_WidgetFeaturePointSelector::mouseReleased(ModuleBase_IViewWindow* theWindow,
                                                       QMouseEvent* theEvent)
{
  // the contex menu release by the right button should not be processed by this widget
  if (theEvent->button() != Qt::LeftButton)
    return;

  std::shared_ptr<ModelAPI_AttributeReference> aRefPreviewAttr =
                          std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                          feature()->data()->attribute(myPreviewObjectAttribute));
  ObjectPtr aPreviewObject = aRefPreviewAttr->value();
  // do not move focus from the current widget if the object is not highlighted/selected
  if (!aPreviewObject.get())
    return;

  // set parameters of preview into parameters of selection in the feature
  std::shared_ptr<ModelAPI_AttributeReference> aRefSelectedAttr =
                          std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                          feature()->data()->attribute(mySelectedObjectAttribute));
  aRefSelectedAttr->setValue(aRefPreviewAttr->value());

  std::shared_ptr<GeomDataAPI_Point2D> aPointSelectedAttr =
                          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                          feature()->data()->attribute(mySelectedPointAttribute));
  std::shared_ptr<GeomDataAPI_Point2D> aPointPreviewAttr =
                          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                          feature()->data()->attribute(myPreviewPointAttribute));
  aPointSelectedAttr->setValue(aPointPreviewAttr->x(), aPointPreviewAttr->y());

  updateObject(feature());

  // we need to deselect base feature for better visibility of selected feature
  XGUI_Tools::workshop(myWorkshop)->displayer()->clearSelected(false);

  // focusOutWidget should be the last functionality in the method because after this emit,
  // the widget may be deleted and members of this class are deleted (e.g. myWorkshop)
  emit focusOutWidget(this);
}

//********************************************************************
bool PartSet_WidgetFeaturePointSelector::fillFeature(
                            const std::shared_ptr<ModuleBase_ViewerPrs>& theSelectedPrs,
                            ModuleBase_IViewWindow* theWindow,
                            QMouseEvent* theEvent)
{
  bool aFilled = false;
  ObjectPtr anObject;
  if (theSelectedPrs.get() && theSelectedPrs->object().get())
    anObject = theSelectedPrs->object();

  std::shared_ptr<ModelAPI_AttributeReference> aRef =
                          std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                          feature()->data()->attribute(myPreviewObjectAttribute));
  aRef->setValue(anObject);

  std::shared_ptr<GeomDataAPI_Point2D> anAttributePoint =
                  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                  feature()->data()->attribute(myPreviewPointAttribute));
  std::shared_ptr<GeomAPI_Pnt2d> aPoint = PartSet_Tools::getPnt2d(theEvent, theWindow, mySketch);
  anAttributePoint->setValue(aPoint);
  // redisplay AIS presentation in viewer
#ifndef HIGHLIGHT_STAYS_PROBLEM
  // an attempt to clear highlighted item in the viewer: but of OCCT
  XGUI_Tools::workshop(myWorkshop)->displayer()->clearSelected(true);
#endif
  updateObject(feature());
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));

  aFilled = true;

  return aFilled;
}

//********************************************************************
QList<ModuleBase_ViewerPrsPtr> PartSet_WidgetFeaturePointSelector::getAttributeSelection() const
{
  return QList<ModuleBase_ViewerPrsPtr>();
}

//********************************************************************
bool PartSet_WidgetFeaturePointSelector::setSelection(
                                          QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                                          const bool theToValidate)
{
  // false is returned to do not emit focus out widget by selected sub-shape
  return false;
}

void PartSet_WidgetFeaturePointSelector::setPreSelection(
                                  const std::shared_ptr<ModuleBase_ViewerPrs>& thePreSelected,
                                  ModuleBase_IViewWindow* theWnd,
                                  QMouseEvent* theEvent)
{
  // the method is empty because firstly by starging of the feature there is no selection of
  // sub-segments in the viewer, secondly preselection of restart operation is processed by
  // special reentrant message sent by the feature
}
