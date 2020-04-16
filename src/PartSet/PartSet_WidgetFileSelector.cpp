// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "PartSet_WidgetFileSelector.h"

#include <ModelAPI_AttributeString.h>

#include <ExchangePlugin_ExportFeature.h>

PartSet_WidgetFileSelector::PartSet_WidgetFileSelector(QWidget* theParent,
                                                       ModuleBase_IWorkshop* theWorkshop,
                                                       const Config_WidgetAPI* theData)
: ModuleBase_WidgetFileSelector(theParent, theData),
  myWorkshop(theWorkshop)
{
}

bool PartSet_WidgetFileSelector::restoreValueCustom()
{
  // A rare case when plugin was not loaded.
  if (!myFeature)
    return false;

  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(ExchangePlugin_ExportFeature::FILE_PATH_ID());
  mySelectedFilter =
    formatToFilter(shortFormatToFullFormat(QString::fromStdString(aStringAttr->value())));

  return ModuleBase_WidgetFileSelector::restoreValueCustom();
}

bool PartSet_WidgetFileSelector::storeValueCustom()
{
  // A rare case when plugin was not loaded.
  if (!myFeature)
    return false;

  DataPtr aData = myFeature->data();
  AttributeStringPtr aStringAttr = aData->string(ExchangePlugin_ExportFeature::FILE_FORMAT_ID());
  aStringAttr->setValue(filterToShortFormat(mySelectedFilter).toStdString());

  return ModuleBase_WidgetFileSelector::storeValueCustom();
}

QString PartSet_WidgetFileSelector::shortFormatToFullFormat( const QString & theFormat ) const
{
  foreach(const QString & eachFormat, getValidatorFormats())
    if (filterToShortFormat(formatToFilter(eachFormat)) == theFormat)
      return eachFormat;
  return QString();
}
