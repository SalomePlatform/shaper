// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef EXCHANGEPLUGIN_IMPORTFEATURE_H_
#define EXCHANGEPLUGIN_IMPORTFEATURE_H_

#include <ExchangePlugin.h>
#include <ModelAPI_Feature.h>

#include <map>

class ExchangePlugin_ImportFeature : public ModelAPI_Feature
{
 public:
  /// Extrusion kind
  inline static const std::string& ID()
  {
    static const std::string MY_IMPORT_ID("Import");
    return MY_IMPORT_ID;
  }
  /// attribute name of referenced face
  inline static const std::string& FILE_PATH_ID()
  {
    static const std::string MY_FILE_PATH_ID("import_file_selector");
    return MY_FILE_PATH_ID;
  }

  EXCHANGEPLUGIN_EXPORT ExchangePlugin_ImportFeature();
  EXCHANGEPLUGIN_EXPORT virtual ~ExchangePlugin_ImportFeature();

  /// Returns the unique kind of a feature
  EXCHANGEPLUGIN_EXPORT virtual const std::string& getKind();

  /// Request for initialization of data model of the feature: adding all attributes
  EXCHANGEPLUGIN_EXPORT virtual void initAttributes();

  /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute();

  EXCHANGEPLUGIN_EXPORT virtual bool isInHistory()
  {
    return true;
  }

 protected:
  EXCHANGEPLUGIN_EXPORT bool importFile(const std::string& theFileName);

private:
  /// Loads Naming data structure to the document
  void loadNamingDS(std::shared_ptr<GeomAPI_Shape> theGeomShape, 
					std::shared_ptr<ModelAPI_ResultBody> theResultBody);
};

#endif /* IMPORT_IMPORTFEATURE_H_ */
