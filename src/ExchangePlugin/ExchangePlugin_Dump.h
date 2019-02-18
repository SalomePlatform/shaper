// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef EXCHANGEPLUGIN_DUMP_H_
#define EXCHANGEPLUGIN_DUMP_H_

#include <ExchangePlugin.h>

#include <ModelAPI_Feature.h>

/**
 * \class ExchangePlugin_Dump
 * \ingroup Plugins
 * \brief Store full model as a Python script
 */
class ExchangePlugin_Dump : public ModelAPI_Feature
{
public:
  /// Feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_DUMP_ID("Dump");
    return MY_DUMP_ID;
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
  /// attribute name of selection type string (GUI radio button) of dumping by topological naming
  inline static const std::string& SELECTION_TYPE_ID()
  {
    static const std::string MY_SELECTION_TYPE_ID("selection_type");
    return MY_SELECTION_TYPE_ID;
  }

  /// string identifier of dumping by topological naming
  inline static const std::string& TOPOLOGICAL_NAMING_DUMP_ID()
  {
    static const std::string MY_TOOPNAMING_DUMP_ID("topological_naming");
    return MY_TOOPNAMING_DUMP_ID;
  }
  /// string identifier of dumping by geometric selection
  inline static const std::string& GEOMETRIC_DUMP_ID()
  {
    static const std::string MY_GEOM_DUMP_ID("geometric_selection");
    return MY_GEOM_DUMP_ID;
  }

  /// string identifier of dumping by using weak naming identifiers of selection
  inline static const std::string& WEAK_NAMING_DUMP_ID()
  {
    static const std::string MY_WEAK_NAMING_DUMP_ID("weak_naming");
    return MY_WEAK_NAMING_DUMP_ID;
  }

  /// Default constructor
  EXCHANGEPLUGIN_EXPORT ExchangePlugin_Dump();
  /// Default destructor
  EXCHANGEPLUGIN_EXPORT virtual ~ExchangePlugin_Dump();

  /// Returns the unique kind of a feature
  EXCHANGEPLUGIN_EXPORT virtual const std::string& getKind()
  {
    return ExchangePlugin_Dump::ID();
  }

  /// Request for initialization of data model of the feature: adding all attributes
  EXCHANGEPLUGIN_EXPORT virtual void initAttributes();

  /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute();

  /// Reimplemented from ModelAPI_Feature::isMacro(). Returns true.
  EXCHANGEPLUGIN_EXPORT virtual bool isMacro() const { return true; }

  /// Reimplemented from ModelAPI_Feature::isPreviewNeeded(). Returns false.
  EXCHANGEPLUGIN_EXPORT virtual bool isPreviewNeeded() const { return false; }

protected:
  /// Performs dump to the file
  EXCHANGEPLUGIN_EXPORT void dump(const std::string& theFileName);
};

#endif
