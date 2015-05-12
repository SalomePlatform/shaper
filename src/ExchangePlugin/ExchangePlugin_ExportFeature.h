// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef EXCHANGEPLUGIN_EXPORTFEATURE_H_
#define EXCHANGEPLUGIN_EXPORTFEATURE_H_

#include <ExchangePlugin.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>

#include <map>

/**\class ExchangePlugin_ExportFeature
 * \ingroup Plugins
 * \brief Feature for export shapes to the external files in CAD formats.
 *
 * The set of supported formats is defined in the configuration file.
 */
class ExchangePlugin_ExportFeature : public ModelAPI_Feature
{
 public:
  /// Extrusion kind
  inline static const std::string& ID()
  {
    static const std::string MY_EXPORT_ID("Export");
    return MY_EXPORT_ID;
  }
  /// attribute name of referenced face
  inline static const std::string& FILE_PATH_ID()
  {
    static const std::string MY_FILE_PATH_ID("export_file_selector");
    return MY_FILE_PATH_ID;
  }
  /// default constructor
  EXCHANGEPLUGIN_EXPORT ExchangePlugin_ExportFeature();
  /// default destructor
  EXCHANGEPLUGIN_EXPORT virtual ~ExchangePlugin_ExportFeature();

  /// Request for initialization of data model of the feature: adding all attributes
  EXCHANGEPLUGIN_EXPORT virtual void initAttributes();

  /// Returns the unique kind of a feature
  EXCHANGEPLUGIN_EXPORT virtual const std::string& getKind();

  /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute();

  /// Reimplemented from ModelAPI_Feature::isMacro(). Returns true.
  EXCHANGEPLUGIN_EXPORT virtual bool isMacro() const { return true; }
  /// Reimplemented from ModelAPI_Feature::isPreviewNeeded(). Returns false.
  MODELAPI_EXPORT virtual bool isPreviewNeeded() const { return false; }

 protected:
  /// Performs the export of the file
  EXCHANGEPLUGIN_EXPORT bool exportFile(const std::string& theFileName);

private:
  /// Loads Naming data structure to the document
  void loadNamingDS(std::shared_ptr<GeomAPI_Shape> theGeomShape, 
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody);
};

#endif /* EXPORT_EXPORTFEATURE_H_ */
