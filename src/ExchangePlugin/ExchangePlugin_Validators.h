// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef EXCHANGEPLUGIN_VALIDATORS_H
#define EXCHANGEPLUGIN_VALIDATORS_H

#include "ExchangePlugin.h"
#include <ModelAPI_AttributeValidator.h>

/**
 * \class ExchangePlugin_FormatValidator
 * \ingroup Validators
 * \brief Validator for the imported formats checking
 *
 * The configuration file of import/export features contains the information
 * about which formats are supported and the extension of the associated files.
 * This validator filters out files that are out of this description.
 */
class ExchangePlugin_FormatValidator : public ModelAPI_AttributeValidator
{
  /**
   * Parses input arguments "BREP:BREPImport", "STEP|STP:STEPImport"
   * into list of file formats "BREP","STEP","STP"
   * and list of corresponding plugins: "BREPImport", "STEPImport"
   */
  static bool parseFormats(const std::list<std::string>& theArguments,
                           std::list<std::string>& outFormats);
public:
  /**
   * Returns true is the file-name attribute correctly corresponds to the set of
   * allowed formats.
   */
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

/**
 * \class ExchangePlugin_ImportFormatValidator
 * \ingroup Validators
 * \brief Validator for the import format.
 */
class ExchangePlugin_ImportFormatValidator : public ExchangePlugin_FormatValidator
{

};

/**
 * \class ExchangePlugin_ExportFormatValidator
 * \ingroup Validators
 * \brief Validator for the export format.
 */
class ExchangePlugin_ExportFormatValidator : public ExchangePlugin_FormatValidator
{

};

/**
 *  Check the selected result is in history (avoid Origin and coordinate axes and planes).
 */
class ExchangePlugin_InHistoryValidator : public ModelAPI_AttributeValidator
{
public:
  /// \return True if the attribute is valid.
  ///         It checks whether the selected object is in history.
  /// \param[in] theAttribute an attribute to check
  /// \param[in] theArguments a filter parameters
  /// \param[out] theError error message.
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
};

#endif
