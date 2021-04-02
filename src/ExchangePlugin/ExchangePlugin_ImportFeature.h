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

#ifndef EXCHANGEPLUGIN_IMPORTFEATURE_H_
#define EXCHANGEPLUGIN_IMPORTFEATURE_H_

#include "ExchangePlugin.h"

#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Result.h>
#include <GeomAPI_Shape.h>

#include <map>

/**
 * \class ExchangePlugin_ImportFeature
 * \ingroup Plugins
 * \brief Feature for import shapes from the external files in CAD formats.
 *
 * The list of supported formats is defined in the configuration file.
 */
class ExchangePlugin_ImportFeatureBase : public ModelAPI_CompositeFeature
{
public:
  /// attribute name of file path
  inline static const std::string& FILE_PATH_ID()
  {
    static const std::string MY_FILE_PATH_ID("file_path");
    return MY_FILE_PATH_ID;
  }
  /// All features (list of references)
  inline static const std::string& FEATURES_ID()
  {
    static const std::string MY_FEATURES_ID("Features");
    return MY_FEATURES_ID;
  }
  /// attribute name of target part
  inline static const std::string& TARGET_PART_ID()
  {
    static const std::string MY_TARGET_PART_ID("target_part");
    return MY_TARGET_PART_ID;
  }
  /// Default constructor
  EXCHANGEPLUGIN_EXPORT ExchangePlugin_ImportFeatureBase() = default;
  /// Default destructor
  EXCHANGEPLUGIN_EXPORT virtual ~ExchangePlugin_ImportFeatureBase() = default;

  /// Returns the unique kind of a feature
  EXCHANGEPLUGIN_EXPORT virtual const std::string& getKind() = 0;

  /// Request for initialization of data model of the feature: adding all attributes
  EXCHANGEPLUGIN_EXPORT virtual void initAttributes();

  /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute() = 0;

  /// Reimplemented from ModelAPI_Feature::isPreviewNeeded(). Returns false.
  EXCHANGEPLUGIN_EXPORT virtual bool isPreviewNeeded() const { return false; }

  /// Reimplemented from ModelAPI_CompositeFeature::addFeature()
  virtual std::shared_ptr<ModelAPI_Feature> addFeature(std::string theID);

  /// Reimplemented from ModelAPI_CompositeFeature::numberOfSubs()
  virtual int numberOfSubs(bool forTree = false) const;

  /// Reimplemented from ModelAPI_CompositeFeature::subFeature()
  virtual std::shared_ptr<ModelAPI_Feature> subFeature(const int theIndex, bool forTree = false);

  /// Reimplemented from ModelAPI_CompositeFeature::subFeatureId()
  virtual int subFeatureId(const int theIndex) const;

  /// Reimplemented from ModelAPI_CompositeFeature::isSub()
  virtual bool isSub(ObjectPtr theObject) const;

  /// Reimplemented from ModelAPI_CompositeFeature::removeFeature()
  virtual void removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature);

 protected:
  /// Performs the import of the file
  EXCHANGEPLUGIN_EXPORT virtual void importFile(const std::string& theFileName) = 0;

  /// Creates and prepares a result body from the shape
  std::shared_ptr<ModelAPI_ResultBody> createResultBody(
      std::shared_ptr<GeomAPI_Shape> aGeomShape);

  /// Loads Naming data structure to the document
  void loadNamingDS(std::shared_ptr<GeomAPI_Shape> theGeomShape,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody);
};

class ExchangePlugin_ImportFeature : public ExchangePlugin_ImportFeatureBase
{
public:
  /// Feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_IMPORT_ID("Import");
    return MY_IMPORT_ID;
  }
  /// Feature kind
  inline static const std::string& IMPORT_TYPE_ID()
  {
    static const std::string MY_IMPORT_TYPE_ID("ImportType");
    return MY_IMPORT_TYPE_ID;
  }
  /// attribute name of file path
  inline static const std::string& STEP_FILE_PATH_ID()
  {
    static const std::string MY_STEP_FILE_PATH_ID("step_file_path");
    return MY_STEP_FILE_PATH_ID;
  }

  /// Default constructor
  EXCHANGEPLUGIN_EXPORT ExchangePlugin_ImportFeature() = default;
  /// Default destructor
  EXCHANGEPLUGIN_EXPORT virtual ~ExchangePlugin_ImportFeature() = default;

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
  /// Returns the unique kind of a feature
  EXCHANGEPLUGIN_EXPORT virtual const std::string& getKind() override
  {
    return ExchangePlugin_ImportFeature::ID();
  }

  /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute() override;

  /// Request for initialization of data model of the feature: adding all attributes
  EXCHANGEPLUGIN_EXPORT virtual void initAttributes();

protected:
  /// Performs the import of the file
  EXCHANGEPLUGIN_EXPORT void importFile(const std::string& theFileName) override;

  /// Performs the import of XAO file
  EXCHANGEPLUGIN_EXPORT void importXAO(const std::string& theFileName);

private:
  /// Set groups of color
  void setColorGroups(std::shared_ptr<ModelAPI_ResultBody> theResultBody);

  /// set a group of color
  void setColorGroup(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                     std::vector<int>& theColor,
                     const std::wstring& theName );

  /// set Materiel group of color
  void setMaterielGroup(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                        std::map< std::wstring, std::list<std::wstring>>& theMaterialShape);

};

class ExchangePlugin_Import_ImageFeature : public ExchangePlugin_ImportFeatureBase
{
 public:
  /// Feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_IMPORT_ID("ImportImage");
    return MY_IMPORT_ID;
  }

  /// Default constructor
  EXCHANGEPLUGIN_EXPORT ExchangePlugin_Import_ImageFeature() = default;
  /// Default destructor
  EXCHANGEPLUGIN_EXPORT virtual ~ExchangePlugin_Import_ImageFeature() = default;

  /// Returns the unique kind of a feature
  EXCHANGEPLUGIN_EXPORT virtual const std::string& getKind() override
  {
    return ExchangePlugin_Import_ImageFeature::ID();
  }

   /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute() override;

protected:
  /// Performs the import of the file
  EXCHANGEPLUGIN_EXPORT void importFile(const std::string& theFileName) override;

};
#endif /* IMPORT_IMPORTFEATURE_H_ */
