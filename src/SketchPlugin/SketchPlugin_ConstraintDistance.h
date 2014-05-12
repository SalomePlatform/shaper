// File:    SketchPlugin_ConstraintDistance.h
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintDistance_HeaderFile
#define SketchPlugin_ConstraintDistance_HeaderFile

#include "SketchPlugin.h"
#include "SketchPlugin_Constraint.h"
#include <list>

/// The distance value
const std::string CONSTRAINT_ATTR_DISTANCE(CONSTRAINT_ATTR_VALUE);


/** \class SketchPlugin_ConstraintDistance
 *  \ingroup DataModel
 *  \brief Feature for creation of a new constraint which fixes the distance 
 *         between two other features.
 *
 *  There are allowed several kinds of distances:
 *    \li The next constraints use "valA", "ptA" and "entityA" parameters:
 *        point-point, point-line, point-plane, point-face;
 *    \li The distance between two points projected on a line (or vector) 
 *        uses "valA", "ptA", "ptB", "entityA";
 *    \li The diameter of a circle is defined by "valA" and "entityA";
 *    \li The angle between two lines (or vectors) uses "valA", "entityA", "entityB".
 *
 *  The default list of attributes contains only CONSTRAINT_ATTR_DISTANCE.
 *
 *  \remark As far as the constraint may have different attributes, 
 *          it is strongly recommended to use setAttibutes() methods 
 *          instead of direct intialization of the attributes
 */
class SketchPlugin_ConstraintDistance: public SketchPlugin_Constraint
{
public:
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind() 
  {static std::string MY_KIND = "SketchConstraintDistance"; return MY_KIND;}

  /// \brief Returns to which group in the document must be added feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getGroup() 
  {static std::string MY_GROUP = "Sketch"; return MY_GROUP;}

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /** \brief Request for initialization of data model of the feature: adding all attributes
   *
   *  The setAttributes() methods should be used instead.
   */
  SKETCHPLUGIN_EXPORT virtual void initAttributes() 
  { /* do nothing */ }

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintDistance();

  /** \brief Initializes the attributes of the constraint
   *
   *  Defines the distance between a point and another entity, 
   *  or defines the angle between two vectors.
   *
   *  \param[in] theDistance distance (or angle) between two entities
   *  \param[in] theEntityA  first parameter of the constraint
   *  \param[in] theEntityB  second parameter of the constraint
   */
  SKETCHPLUGIN_EXPORT void setAttributes(
          const double                                  theDistance, 
          const boost::shared_ptr<SketchPlugin_Feature> theEntityA, 
          const boost::shared_ptr<SketchPlugin_Feature> theEntityB);
  /** \brief Initializes the attributes of the constraint
   *
   *  Defines the distance between a point defined by reference and another entity.
   *
   *  \param[in] theDistance distance between the point and other entity
   *  \param[in] thePoint    reference to the point attribute
   *  \param[in] theEntity   other parameter of the constraint
   */
  SKETCHPLUGIN_EXPORT void setAttributes(
          const double                                  theDistance, 
          const boost::shared_ptr<ModelAPI_Attribute>   thePoint, 
          const boost::shared_ptr<SketchPlugin_Feature> theEntity);
  /** \brief Initializes the attributes of the constraint
   *
   *  Defines the distance between two points which defined by references.
   *
   *  \param[in] theDistance distance between the points
   *  \param[in] thePointA   reference to the first point attribute
   *  \param[in] thePointB   reference to the second point attribute
   */
  SKETCHPLUGIN_EXPORT void setAttributes(
          const double                                  theDistance, 
          const boost::shared_ptr<ModelAPI_Attribute>   thePointA, 
          const boost::shared_ptr<ModelAPI_Attribute>   thePointB);
  /** \brief Initializes the attributes of the constraint
   *
   *  Defines radius of a circle.
   *
   *  \param[in] theRadius radius of the circle
   *  \param[in] theCircle the circle which has specified radius
   */
  SKETCHPLUGIN_EXPORT void setAttributes(
          const double                                  theRadius, 
          const boost::shared_ptr<SketchPlugin_Feature> theCircle);
  /** \brief Initializes the attributes of the constraint
   *
   *  Defines the distance between two points projected on specified vector (or line).
   *
   *  \param[in] theDistance distance between projected points
   *  \param[in] thePointA   first point for the projection
   *  \param[in] thePointB   second point for the projection
   *  \param[in] theEntity   direction of the line which the points projected on
   */
  SKETCHPLUGIN_EXPORT void setAttributes(
          const double                                  theDistance, 
          const boost::shared_ptr<SketchPlugin_Feature> thePointA, 
          const boost::shared_ptr<SketchPlugin_Feature> thePointB, 
          const boost::shared_ptr<SketchPlugin_Feature> theEntity);

private:
  /** \brief Returns the list of attributes for the certain type of constraint.
   *  \return names of attributes
   */
  const std::list<std::string>& getAttributesList() const
  { return myAttrList; }

private:
  std::list<std::string> myAttrList; ///< List of attributes for current constraint
};

#endif
