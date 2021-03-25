// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef EXCHANGEPLUGIN_EXPORTPART_H_
#define EXCHANGEPLUGIN_EXPORTPART_H_

#include <ExchangePlugin.h>
#include <ModelAPI_Feature.h>

/**
 * \class ExchangePlugin_ExportPart
 * \ingroup Plugins
 * \brief Feature for export some results of a Part to the binary format for the further import.
 */
class ExchangePlugin_ExportPart : public ModelAPI_Feature
{
public:
  /// Feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_EXPORT_ID("ExportPart");
    return MY_EXPORT_ID;
  }
  /// attribute name of file path
  inline static const std::string& FILE_PATH_ID()
  {
    static const std::string MY_FILE_PATH_ID("file_path");
    return MY_FILE_PATH_ID;
  }
  /// attribute name of file format
  inline static const std::string& FILE_FORMAT_ID()
  {
    static const std::string MY_FILE_FORMAT_ID("file_format");
    return MY_FILE_FORMAT_ID;
  }
  /// attribute name of selection list
  inline static const std::string& SELECTION_LIST_ID()
  {
    static const std::string MY_SELECTION_LIST_ID("selection_list");
    return MY_SELECTION_LIST_ID;
  }
  /// Default constructor
  ExchangePlugin_ExportPart();

  /// Returns the unique kind of a feature
  EXCHANGEPLUGIN_EXPORT virtual const std::string& getKind()
  {
    return ExchangePlugin_ExportPart::ID();
  }

  /// Request for initialization of data model of the feature: adding all attributes
  EXCHANGEPLUGIN_EXPORT virtual void initAttributes();

  /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute();

  /// Returns true if this feature is used as macro: creates other features and then removed.
  EXCHANGEPLUGIN_EXPORT virtual bool isMacro() const { return true; }

  /// Reimplemented from ModelAPI_Feature::isPreviewNeeded(). Returns false.
  EXCHANGEPLUGIN_EXPORT virtual bool isPreviewNeeded() const { return false; }

  /// Do not put in history.
  /// Since it is not a macro, it is not deleted, but we don't want to see it.
  bool isInHistory()  { return false; }
};

#endif /* EXCHANGEPLUGIN_EXPORTPART_H_ */
