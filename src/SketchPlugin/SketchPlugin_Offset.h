// Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_Offset_H_
#define SketchPlugin_Offset_H_

#include <SketchPlugin.h>
#include <SketchPlugin_ConstraintBase.h>

#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Edge.h>

class GeomAlgoAPI_MakeShape;
class GeomAlgoAPI_Offset;
class GeomAlgoAPI_WireBuilder;

/**\class SketchPlugin_Offset
 * \ingroup Plugins
 * \brief Builds offset curves in the sketch.
 */
class SketchPlugin_Offset : public SketchPlugin_ConstraintBase
{
public:
  /// Offset macro feature kind
  inline static const std::string& ID()
  {
    static const std::string ID("SketchOffset");
    return ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Offset::ID();
    return MY_KIND;
  }

  /// Type of joint
  inline static const std::string& JOINT_ID()
  {
    static const std::string ID("offset_joint");
    return ID;
  }

  /// Keep distance joint (add arcs where needed)
  inline static const std::string& JOINT_KEEP_DISTANCE()
  {
    static const std::string ID("KeepDistance");
    return ID;
  }

  /// Arcs joint (make fillets on all straight lines intersections)
  inline static const std::string& JOINT_ARCS()
  {
    static const std::string ID("Arcs");
    return ID;
  }

  /// Lines joint (do not add new arcs, prolongate and intersect adjacent lines)
  inline static const std::string& JOINT_LINES()
  {
    static const std::string ID("Lines");
    return ID;
  }

  /// list of offset edges
  inline static const std::string& EDGES_ID()
  {
    static const std::string ID("segments");
    return ID;
  }

  /// attribute to store the offset value
  inline static const std::string& VALUE_ID()
  {
    static const std::string ID("offset_value");
    return ID;
  }

  /// attribute to store the reversed offset direction
  inline static const std::string& REVERSED_ID()
  {
    static const std::string ID("reversed");
    return ID;
  }

  /// attribute to approx by segments and arcs
  inline static const std::string& APPROX_ID()
  {
    static const std::string ID("approx");
    return ID;
  }

  /// name for add wire action
  inline static const std::string& ADD_WIRE_ACTION_ID()
  {
    static const std::string ID("add_wire");
    return ID;
  }

  /// Called on change of any argument-attribute of this object
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const { return true; }

  /// Find edges connected by coincident boundary constraint and composing a wire with
  /// the already selected segments. It means that not more than 2 edges can be connected
  /// with the coincident point.
  /// \param[in] theActionId action key id (in following form: Action#Index)
  /// \return \c false in case the action not performed.
  SKETCHPLUGIN_EXPORT virtual bool customAction(const std::string& theActionId);

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Use plugin manager for features creation.
  SketchPlugin_Offset();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initAttributes();

private:
  /// Find all wires connected with the selected edges
  bool findWires();

  /// Create sketch feature for each edge of the offset result,
  /// and store it in ENTITY_B(). Original edges are copied to ENTITY_A() to update
  /// correctly if original selection is modified.
  void addToSketch (const std::list< std::shared_ptr<GeomAlgoAPI_MakeShape> >& theOffsetAlgos);

  /// Create BSpline or BSplinePeriodic sketch feature from theEdge
  void mkBSpline (FeaturePtr& theResult, const GeomEdgePtr& theEdge,
                  std::list<ObjectPtr>& thePoolOfFeatures);

  /// Update existent feature by the parameters of the given edge or create a new feature.
  /// \param[in]     theShape          a shape to be converted to sketch feature
  /// \param[in,out] theFeature        sketch feature to be updated or created from scratch
  /// \param[in,out] thePoolOfFeatures list of features to be removed (may be used as a new feature)
  void updateExistentOrCreateNew (const GeomShapePtr& theShape, FeaturePtr& theFeature,
                                  std::list<ObjectPtr>& thePoolOfFeatures);

  /// Find edges that prolongate theEdgeFeature (in a chain) at theEndPoint
  /// Recursive method.
  /// \param[in] theFirstEdge Start edge of wire searching
  /// \param[in] theEdge Current edge
  /// \param[in] theEndPoint Point of the Current edge, not belonging to a previous edge
  /// \param[in/out] theEdgesSet All edges to find among. If empty, all sketch edges assumed.
  /// \param[in/out] theProcessedEdgesSet Already processed (put in chains) edges.
  /// \param[in/out] theChain Resulting edges
  /// \param[in] isPrepend if true, push new found edges to theChain front, else to the back
  /// \return \c true if the chain is closed
  bool findWireOneWay (const FeaturePtr& theFirstEdge,
                       const FeaturePtr& theEdge,
                       const std::shared_ptr<GeomDataAPI_Point2D>& theEndPoint,
                       std::set<FeaturePtr>& theEdgesSet,
                       std::set<FeaturePtr>& theProcessedEdgesSet,
                       std::list<FeaturePtr>& theChain,
                       const bool isPrepend = false);

  void makeFillet (const double theValue,
                   const std::shared_ptr<GeomAlgoAPI_WireBuilder>&,
                   const std::shared_ptr<GeomAlgoAPI_Offset>&,
                   std::list< std::shared_ptr<GeomAlgoAPI_MakeShape> >& theOffsetAlgos);
};

#endif
