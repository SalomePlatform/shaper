// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_DimensionStyleListener.h
// Created:     31 March 2016
// Author:      Natalia ERMOLAEVA

#ifndef SketcherPrs_DimensionStyleListener_H
#define SketcherPrs_DimensionStyleListener_H

#include <Events_Listener.h>

#include <ModelAPI_AttributeDouble.h>

#include "SketcherPrs_Tools.h"

#include <Standard.hxx>

#include <string>

/**
* \ingroup GUI
* A class for representation of linear dimension constraint.
* It supports SketchPlugin_ConstraintLength and SketchPlugin_ConstraintDistance features.
*/
class SketcherPrs_DimensionStyleListener : public Events_Listener
{
public:
  class DimensionValue {
  public:
    DimensionValue(double theDoubleValue, bool theHasParameters, const std::string& theTextValue);
    /// Fills internal fields by the given attribute
    /// \param theAttributeValue a model attribute
    void init(const AttributeDoublePtr& theAttributeValue);

  public:
    double myDoubleValue; ///< the angle value to be shown as custom value of presentation
    bool myHasParameters; ///< true if the atrribute value has used parameters
    std::string myTextValue; ///< the angle value depending on angle type
  };

public:
  /// Constructor
  Standard_EXPORT SketcherPrs_DimensionStyleListener();

  /// Destructor
  Standard_EXPORT ~SketcherPrs_DimensionStyleListener();

  /// Process the ModelAPI_DocumentCreatedMessage to fulfill a document
  /// from the message with origin and planes
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  /// Visualizes the dimension text or dimension value depending on the has parameters state
  /// \param theDimension a modified dimension
  /// \param theDimensionValue container filled by the model double attribute
  Standard_EXPORT void updateDimensions(AIS_Dimension* theDimension,
                                        const DimensionValue& theDimensionValue);

private:
  /// Visualizes the dimension text or dimension value depending on the has parameters state
  /// \param theDimension a modified dimension
  /// \param theHasParameters if true, the text is shown, else digit
  /// \param theTextValue a dimension text value
  /// \param theDoubleValue a dimension digit value
  void updateDimensions(AIS_Dimension* theDimension,
                        const bool theHasParameters,
                        const std::string& theTextValue,
                        const double theDoubleValue);
private:
  /// Style how the parameter of dimension should be visualized
  SketcherPrs_ParameterStyleMessage::ParameterStyle myStyle;
};

#endif