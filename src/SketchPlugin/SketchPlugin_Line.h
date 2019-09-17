// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_Line_H_
#define SketchPlugin_Line_H_

#include <ModelAPI_IReentrant.h>

#include "SketchPlugin.h"
#include <SketchPlugin_SketchEntity.h>
#include <SketchPlugin_Sketch.h>
#include <list>

class GeomAPI_Pnt2d;

/**\class SketchPlugin_Line
 * \ingroup Plugins
 * \brief Feature for creation of the new part in PartSet.
 */
class SketchPlugin_Line : public SketchPlugin_SketchEntity,
                          public ModelAPI_IReentrant
{
 public:
  /// Arc feature kind
  inline static const std::string& ID()
  {
    static const std::string SKETCH_LINE_ID("SketchLine");
    return SKETCH_LINE_ID;
  }
  /// Start 2D point of the line
  inline static const std::string& START_ID()
  {
    static const std::string MY_START_ID("StartPoint");
    return MY_START_ID;
  }
  /// End 2D point of the line
  inline static const std::string& END_ID()
  {
    static const std::string MY_END_ID("EndPoint");
    return MY_END_ID;
  }

  /// Line length.
  static const std::string& LENGTH_ID()
  {
    static const std::string MY_LENGTH("LineLength");
    return MY_LENGTH;
  }

  /// Reference to the parent feature
  inline static const std::string& PARENT_ID()
  {
    static const std::string& MY_PARENT_ID("ParentFeature");
    return MY_PARENT_ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind();

  /// Returns true is sketch element is under the rigid constraint
  SKETCHPLUGIN_EXPORT virtual bool isFixed();

  /// Request for initialization of data model of the feature: adding all attributes
  virtual void initAttributes();

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Apply information of the message to current object. It fills start attribute of
  /// the current feature by last attribute of the message feature, build coincidence
  /// if message has selected object
  virtual std::string processEvent(const std::shared_ptr<Events_Message>& theMessage);

  /// Return the distance between the feature and the point
  /// \param thePoint the point
  double distanceToPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// Called on change of any argument-attribute of this object
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Use plugin manager for features creation
  SketchPlugin_Line();

private:
  /// Calculates the length of the line and fill the length attribute with the value
  void updateLenghtValue();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();
};

#endif
