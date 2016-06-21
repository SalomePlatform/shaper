// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    ExchangePlugin_ImportFeature.h
// Created: Aug 28, 2014
// Authors:  Sergey BELASH, Sergey POKHODENKO

#ifndef EXCHANGEPLUGIN_IMPORTFEATURE_H_
#define EXCHANGEPLUGIN_IMPORTFEATURE_H_

#include "ExchangePlugin.h"

#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Result.h>

#include <map>

/**
 * \class ExchangePlugin_ImportFeature
 * \ingroup Plugins
 * \brief Feature for import shapes from the external files in CAD formats.
 *
 * The list of supported formats is defined in the configuration file.
 */
class ExchangePlugin_ImportFeature : public ModelAPI_CompositeFeature
{
 public:
  /// Feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_IMPORT_ID("Import");
    return MY_IMPORT_ID;
  }
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
  /// Default constructor
  EXCHANGEPLUGIN_EXPORT ExchangePlugin_ImportFeature();
  /// Default destructor
  EXCHANGEPLUGIN_EXPORT virtual ~ExchangePlugin_ImportFeature();

  /// Returns the unique kind of a feature
  EXCHANGEPLUGIN_EXPORT virtual const std::string& getKind()
  {
    return ExchangePlugin_ImportFeature::ID();
  }

  /// Request for initialization of data model of the feature: adding all attributes
  EXCHANGEPLUGIN_EXPORT virtual void initAttributes();

  /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute();

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
  EXCHANGEPLUGIN_EXPORT void importFile(const std::string& theFileName);

  /// Performs the import of XAO file
  EXCHANGEPLUGIN_EXPORT void importXAO(const std::string& theFileName);

  /// Creates and prepares a result body from the shape
  std::shared_ptr<ModelAPI_ResultBody> createResultBody(
      std::shared_ptr<GeomAPI_Shape> aGeomShape);

private:
  /// Loads Naming data structure to the document
  void loadNamingDS(std::shared_ptr<GeomAPI_Shape> theGeomShape, 
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody);
};

#endif /* IMPORT_IMPORTFEATURE_H_ */
