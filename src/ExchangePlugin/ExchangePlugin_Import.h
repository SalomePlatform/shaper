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

#ifndef EXCHANGEPLUGIN_IMPORT_H_
#define EXCHANGEPLUGIN_IMPORT_H_

#include "ExchangePlugin.h"

#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeStringArray.h>
#include <map>


/**
 * \class ExchangePlugin_ImportFeature
 * \ingroup Plugins
 * \brief Feature for import shapes from the external files in CAD formats.
 *
 * The list of supported formats is defined in the configuration file.
 */
class ExchangePlugin_ImportBase : public ModelAPI_Feature
{
 public:
  /// attribute name of file path
  inline static const std::string& FILE_PATH_ID()
  {
    static const std::string MY_FILE_PATH_ID("file_path");
    return MY_FILE_PATH_ID;
  }
  /// attribute name of target part
  inline static const std::string& TARGET_PART_ID()
  {
    static const std::string MY_TARGET_PART_ID("target_part");
    return MY_TARGET_PART_ID;
  }
  /// attribute name of list of target parts
  inline static const std::string& TARGET_PARTS_LIST_ID()
  {
    static const std::string MY_TARGET_PARTS_LIST_ID("target_parts_list");
    return MY_TARGET_PARTS_LIST_ID;
  }
  /// Default constructor
  EXCHANGEPLUGIN_EXPORT ExchangePlugin_ImportBase()= default;
  /// Default destructor
  EXCHANGEPLUGIN_EXPORT virtual ~ExchangePlugin_ImportBase()= default;


  /// Request for initialization of data model of the feature: adding all attributes
  EXCHANGEPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  EXCHANGEPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID) = 0;

  /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute() = 0;

  /// Returns true if this feature is used as macro: creates other features and then removed.
  EXCHANGEPLUGIN_EXPORT virtual bool isMacro() const { return true; }

  /// Reimplemented from ModelAPI_Feature::isPreviewNeeded(). Returns false.
  EXCHANGEPLUGIN_EXPORT virtual bool isPreviewNeeded() const { return false; }

 protected:

  EXCHANGEPLUGIN_EXPORT void  updatePart(AttributeStringArrayPtr& aPartsAttr,
                                         AttributeIntegerPtr& aTargetAttr);
};

EXCHANGEPLUGIN_EXPORT class ExchangePlugin_Import : public ExchangePlugin_ImportBase
{
 public:
  /// Feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_IMPORT_ID("ImportMacro");
    return MY_IMPORT_ID;
  }
 /// Feature kind
  inline static const std::string& IMPORT_TYPE_ID()
  {
    static const std::string MY_IMPORT_TYPE_ID("ImportType");
    return MY_IMPORT_TYPE_ID;
  }
 /// attribute name of step file path
  inline static const std::string& STEP_FILE_PATH_ID()
  {
    static const std::string MY_STEP_FILE_PATH_ID("step_file_path");
    return MY_STEP_FILE_PATH_ID;
  }
  /// attribute name of step target part
  inline static const std::string& STEP_TARGET_PART_ID()
  {
    static const std::string MY_STEP_TARGET_PART_ID("step_target_part");
    return MY_STEP_TARGET_PART_ID;
  }
  /// attribute name of list ofstep target parts
  inline static const std::string& STEP_TARGET_PARTS_LIST_ID()
  {
    static const std::string MY_STEP_TARGET_PARTS_LIST_ID("step_target_parts_list");
    return MY_STEP_TARGET_PARTS_LIST_ID;
  }
  /// attribute name of step Scale to International System Units
  inline static const std::string& STEP_SCALE_INTER_UNITS_ID()
  {
    static const std::string MY_STEP_SCALE_INTER_UNITS_ID("step_scale_inter_units");
    return MY_STEP_SCALE_INTER_UNITS_ID;
  }
  /// attribute name of step materiels
  inline static const std::string& STEP_MATERIALS_ID()
  {
    static const std::string MY_STEP_MATERIALS_ID("step_materials");
    return MY_STEP_MATERIALS_ID;
  }
  /// attribute name of step colors
  inline static const std::string& STEP_COLORS_ID()
  {
    static const std::string MY_STEP_COLORS_ID("step_colors");
    return MY_STEP_COLORS_ID;
  }
  /// Default constructor
  EXCHANGEPLUGIN_EXPORT ExchangePlugin_Import() = default;
  /// Default destructor
  EXCHANGEPLUGIN_EXPORT virtual ~ExchangePlugin_Import() = default;

  /// Returns the unique kind of a feature
  EXCHANGEPLUGIN_EXPORT virtual const std::string& getKind() override
  {
    return ExchangePlugin_Import::ID();
  }
  /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute() override;

  /// Request for initialization of data model of the feature: adding all attributes
  EXCHANGEPLUGIN_EXPORT virtual void initAttributes();

  // Called on change of any argument-attribute of this object
    /// \param theID identifier of changed attribute
  EXCHANGEPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID) override;

};

EXCHANGEPLUGIN_EXPORT class ExchangePlugin_Import_Image : public ExchangePlugin_ImportBase
{
 public:
  /// Feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_IMPORT_ID("ImportImageMacro");
    return MY_IMPORT_ID;
  }

  /// Default constructor
  EXCHANGEPLUGIN_EXPORT ExchangePlugin_Import_Image() = default;
  /// Default destructor
  EXCHANGEPLUGIN_EXPORT virtual ~ExchangePlugin_Import_Image() = default;

  /// Returns the unique kind of a feature
  EXCHANGEPLUGIN_EXPORT virtual const std::string& getKind() override
  {
    return ExchangePlugin_Import_Image::ID();
  }

  /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute() override;

  // Called on change of any argument-attribute of this object
    /// \param theID identifier of changed attribute
  EXCHANGEPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID) override;

};

#endif /* IMPORT_IMPORTFEATURE_H_ */
