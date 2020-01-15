// Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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

#include <SketchPlugin_WidgetCreator.h>
#include <SketchPlugin_BSplineWidget.h>


SketchPlugin_WidgetCreator::SketchPlugin_WidgetCreator()
  : ModuleBase_IWidgetCreator()
{
  myPanelTypes.insert("bspline-panel");
}

void SketchPlugin_WidgetCreator::panelTypes(std::set<std::string>& theTypes)
{
  theTypes = myPanelTypes;
}

QWidget* SketchPlugin_WidgetCreator::createPanelByType(
    const std::string& theType,
    QWidget* theParent,
    const FeaturePtr& theFeature,
    Config_WidgetAPI* theWidgetApi)
{
  QWidget* aWidget = 0;
  if (theType == "bspline-panel") {
    SketchPlugin_BSplineWidget* aPanel = new SketchPlugin_BSplineWidget(theParent, theWidgetApi);
    aPanel->setFeature(theFeature);
    aWidget = aPanel;
  }
  return aWidget;
}
