// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Tools.h
// Created:     10 March 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_Tools_H
#define SketcherPrs_Tools_H

#include "SketcherPrs.h"
#include <GeomAPI_Shape.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Ax3.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Feature.h>
#include <string>

#include <Events_Loop.h>
#include <Events_Message.h>

class GeomDataAPI_Point2D;
class AIS_Dimension;

//#define MyTextHeight 20

/// Message that document (Part, PartSet) was created
class SketcherPrs_ParameterStyleMessage : public Events_Message
{
public:
  enum ParameterStyle {
    ParameterValue, /// using symbol with the parameter value
    ParameterText /// using parameter text
  };

public:
  /// Creates an empty message
  SKETCHERPRS_EXPORT SketcherPrs_ParameterStyleMessage(const Events_ID theID,
                                                    const void* theSender = 0)
  : Events_Message(theID, theSender) {}
  /// The virtual destructor
  SKETCHERPRS_EXPORT virtual ~SketcherPrs_ParameterStyleMessage() {}
  /// Static. Returns EventID of the message.
  SKETCHERPRS_EXPORT static Events_ID eventId()
  {
    static const char * MY_EVENT_PARAMETER_STYLE_ID("ParameterStyle");
    return Events_Loop::eventByName(MY_EVENT_PARAMETER_STYLE_ID);
  }
  /// Returns a document stored in the message
  SKETCHERPRS_EXPORT ParameterStyle style() const { return myStyle; }
  /// Sets a document to the message
  SKETCHERPRS_EXPORT void setStyle(ParameterStyle theStyle) { myStyle = theStyle; }
private:
  ParameterStyle myStyle; /// style of the parameter visualization
};

namespace SketcherPrs_Tools {

  /// Enumeration with modes for activation of selection custom presentations
  enum SelectionModes {
    /// Start of enumeration
    Sel_Mode_First = 100, 
  
    /// Selection mode for all constraints exclude dimensions
    Sel_Constraint,
  
    /// Selection mode for whole dimension
    Sel_Dimension_All,
  
    /// Selection mode for line of dimension
    Sel_Dimension_Line,

    /// Selection mode foe text of dimension
    Sel_Dimension_Text
  };

  /// Type of angle
  enum AngleType{
    ANGLE_DIRECT,   ///< Angle from the first line to the second line
    ANGLE_COMPLEMENTARY,  ///< Additional angle to the angle from first to second line
    ANGLE_BACKWARD ///< Angle from the second line to the first line
  };

  /// Event ID about expression visual state has been changed, the symbol with a digit
  /// or parameter text is shown

  /// Returns attribute object referenced by feature
  /// \param theFeature a feature
  /// \param theAttrName an attribute name
  SKETCHERPRS_EXPORT AttributePtr getAttribute(ModelAPI_Feature* theFeature,
                                               const std::string& theAttrName);

  /// Returns result object referenced by feature
  /// \param theFeature a feature
  /// \param theAttrName an attribute name
  SKETCHERPRS_EXPORT ObjectPtr getResult(ModelAPI_Feature* theFeature,
                                         const std::string& theAttrName);

  /// Returns shape from result object (or NULL)
  /// \param theObject a result object
  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Shape> getShape(ObjectPtr theObject);


  /// Returns point from a line feature
  /// \param theFeature a line feature
  /// \param theAttrName an attribute of the point
  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Pnt2d> getPoint(ModelAPI_Feature* theFeature,
                                          const std::string& theAttrName);


  /// Returns point projected on a line
  /// \param theLine  a line
  /// \param thePoint a projecting point
  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Pnt2d> getProjectionPoint(
                        const FeaturePtr theLine,
                        const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  SKETCHERPRS_EXPORT FeaturePtr getFeatureLine(DataPtr theData,
                                               const std::string& theAttribute);

  /// Obtain the point object from specified constraint parameter
  /// \param theData a data object
  /// \param theAttribute an attribute name
  /// \param thePlane a projection plane (sketcher plane)
  SKETCHERPRS_EXPORT std::shared_ptr<GeomDataAPI_Point2D> getFeaturePoint(DataPtr theData,
                                                       const std::string& theAttribute,
                                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  /// Returns value of dimension arrows size
  SKETCHERPRS_EXPORT double getArrowSize();

  /// Set value of dimension arrows size
  /// \param theSize a size value
  SKETCHERPRS_EXPORT void setArrowSize(double theSize);

  /// Returns default value of dimension arrows size
  SKETCHERPRS_EXPORT int getDefaultArrowSize();

  /// Set value of dimension text height
  /// \param theHeight a height value
  SKETCHERPRS_EXPORT void setTextHeight(double theHeight);

  /// Returns value of dimension text height
  SKETCHERPRS_EXPORT double getTextHeight();

  /// Returns default value of dimension text height
  SKETCHERPRS_EXPORT double getDefaultTextHeight();

  /// Get flayout distance of the dimension constraint
  /// \param theConstraint a dimension constraint object
  SKETCHERPRS_EXPORT double getFlyoutDistance(const ModelAPI_Feature* theConstraint);

  /// Compute anchor pint for radius dimension
  SKETCHERPRS_EXPORT std::shared_ptr<GeomAPI_Pnt> getAnchorPoint(
                                              const ModelAPI_Feature* theConstraint,
                                              const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  /// Display/hide sigma symbol in the dimension presentation
  /// \param theDimension a dimension constraint
  /// \param theToDisplay a boolean value
  SKETCHERPRS_EXPORT void setDisplaySpecialSymbol(AIS_Dimension* theDimension,
                                                  const bool& theToDisplay);

  /// Display the parameter value instead of dimention digit
  /// \param theDimension a dimension constraint
  /// \param theParameter a parameter value
  /// \param theToDisplay a boolean value
  SKETCHERPRS_EXPORT void setDisplayParameter(AIS_Dimension* theDimension,
                                              const std::string& theParameter,
                                              const bool& theToDisplay);

  /// Sends event about expression visualization type is changed for dimension presentations
  /// Sends event to redisplay all sub-features of composite feature
  /// \param theState a new state
  SKETCHERPRS_EXPORT void sendExpressionShownEvent(const bool& theState);

};

#endif
