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

#include "FiltersAPI_Selection.h"

FiltersAPI_Selection::FiltersAPI_Selection(const FiltersPtr & theFeature)
{
  myVariantType = VT_Filtering;
  myFilterFeature = std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(theFeature->feature());
}

FiltersAPI_Selection::~FiltersAPI_Selection()
{
}

FiltersFeaturePtr FiltersAPI_Selection::feature() const
{
  return myFilterFeature;
}

// ================================================================================================
FiltersAPI_Selection filters(const std::shared_ptr<ModelAPI_Document>& thePart,
                             const std::list<FilterAPIPtr>& theFilters)
{
  FeaturePtr aFeature = thePart->addFeature(FiltersPlugin_Selection::ID());
  FiltersPtr aFiltersFeature(new FiltersAPI_Feature(aFeature));
  aFiltersFeature->setFilters(theFilters);
  return FiltersAPI_Selection(aFiltersFeature);
}
