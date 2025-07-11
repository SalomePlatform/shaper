// Copyright (C) 2014-2025  CEA, EDF
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

#ifndef FILTERSAPI_ARGUMENT_H_
#define FILTERSAPI_ARGUMENT_H_

#include "FiltersAPI.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelHighAPI_Double.h>

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>

#include <limits>

/**\class FiltersAPI_Argument
 * \ingroup CPPHighAPI
 * \brief Argument of the Filter
 */
class FiltersAPI_Argument
{
public:
  FILTERSAPI_EXPORT FiltersAPI_Argument();

  FILTERSAPI_EXPORT
  FiltersAPI_Argument(const bool theValue);

  FILTERSAPI_EXPORT
  FiltersAPI_Argument(const ModelHighAPI_Double theValue);

  FILTERSAPI_EXPORT
  FiltersAPI_Argument(const double& theValue);

  FILTERSAPI_EXPORT
  FiltersAPI_Argument(const std::string& theValue);

  FILTERSAPI_EXPORT
  FiltersAPI_Argument(const ModelHighAPI_Selection& theSelection);

  FILTERSAPI_EXPORT
  FiltersAPI_Argument(const AttributeSelectionPtr& theSelection);

  FILTERSAPI_EXPORT
  FiltersAPI_Argument(const AttributeSelectionListPtr& theSelectionList);

  /// Destructor
  FILTERSAPI_EXPORT
  virtual ~FiltersAPI_Argument();

  const bool boolean() const { return myBoolean; }
  const std::string& string() const { return myValue; }
  const ModelHighAPI_Selection& selection() const { return mySelection; }
  const ModelHighAPI_Double& dble() const { return myDouble; }
  /// Dump wrapped feature
  FILTERSAPI_EXPORT
  void dump(ModelHighAPI_Dumper& theDumper) const;

private:
  bool myBoolean;
  ModelHighAPI_Double myDouble  = std::numeric_limits<double>::lowest() ;
  std::string myValue;
  ModelHighAPI_Selection mySelection;
  AttributeSelectionPtr mySelectionAttr;
  AttributeSelectionListPtr mySelectionListAttr;
};

#endif
