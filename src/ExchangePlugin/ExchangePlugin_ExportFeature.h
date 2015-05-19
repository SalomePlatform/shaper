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
  inline static const std::string& ID()
  {
    static const std::string MY_EXPORT_ID("Export");
    return MY_EXPORT_ID;
  }
  /// attribute name of file format
  inline static const std::string& FILE_FORMAT_ID()
  {
    static const std::string MY_FILE_FORMAT_ID("export_file_format");
    return MY_FILE_FORMAT_ID;
  }
  /// attribute name of file path
  inline static const std::string& FILE_PATH_ID()
  {
    static const std::string MY_FILE_PATH_ID("export_file_selector");
    return MY_FILE_PATH_ID;
  }
  /// attribute name of selection list
  inline static const std::string& SELECTION_LIST_ID()
  {
    static const std::string MY_SELECTION_LIST_ID("selection_list");
    return MY_SELECTION_LIST_ID;
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
  EXCHANGEPLUGIN_EXPORT virtual bool isPreviewNeeded() const { return false; }

protected:
  /// Performs the export of the file
  EXCHANGEPLUGIN_EXPORT bool exportFile(const std::string& theFileName,
                                        const std::string& theFormat,
                                        std::shared_ptr<GeomAPI_Shape> theShape);
};

#endif /* EXPORT_EXPORTFEATURE_H_ */
