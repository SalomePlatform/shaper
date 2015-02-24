// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef INITIALIZATIONPLUGIN_ORIGINPLANESFEATURE_H_
#define INITIALIZATIONPLUGIN_ORIGINPLANESFEATURE_H_

#include <InitializationPlugin.h>
#include <ModelAPI_Feature.h>

#include <map>

class InitializationPlugin_OriginPlanesFeature : public ModelAPI_Feature
{
 public:
  explicit InitializationPlugin_OriginPlanesFeature();
  virtual ~InitializationPlugin_OriginPlanesFeature();
  /// Extrusion kind
  inline static const std::string& ID()
  {
    static const std::string MY_ORIGIN_PLANES_ID("OriginAndPlanes");
    return MY_ORIGIN_PLANES_ID;
  }

  INITIALIZATIONPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = InitializationPlugin_OriginPlanesFeature::ID();
    return MY_KIND;
  }

  INITIALIZATIONPLUGIN_EXPORT virtual void initAttributes();

  INITIALIZATIONPLUGIN_EXPORT virtual void execute();

  INITIALIZATIONPLUGIN_EXPORT virtual bool isInHistory()
  {
    return false;
  }

 protected:
  void createPoint(const std::shared_ptr<ModelAPI_Document>& aDoc);
};

#endif /* IMPORT_IMPORTFEATURE_H_ */
