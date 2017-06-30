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

#ifndef EXCHANGEPLUGIN_TOOLS_H_
#define EXCHANGEPLUGIN_TOOLS_H_

#include <ExchangePlugin.h>
#include <ModelAPI_AttributeTables.h>

#include <list>
#include <string>

/**
 * \class ExchangePlugin_Tools
 * \brief Internal tools for the ExchangePlugin.
 */
class EXCHANGEPLUGIN_EXPORT ExchangePlugin_Tools {
public:
  /// Splits theString using theDelimiter.
  static std::list<std::string> split(const std::string& theString,
                                      char theDelimiter);

  /// Converts string representation of selection type to XAO dimension.
  static std::string selectionType2xaoDimension(const std::string& theString);

  /// Converts string representation of XAO dimension to selection type.
  static std::string xaoDimension2selectionType(const std::string& theDimension);

  /// Converts representation of values type to XAO type.
  static std::string valuesType2xaoType(const ModelAPI_AttributeTables::ValueType& theType);
  /// Converts representation of values type to XAO type.
  static ModelAPI_AttributeTables::ValueType xaoType2valuesType(std::string theType);
};

#endif /* EXCHANGEPLUGIN_TOOLS_H_ */
