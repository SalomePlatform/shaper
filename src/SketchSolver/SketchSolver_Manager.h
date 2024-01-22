// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef SketchSolver_Manager_H_
#define SketchSolver_Manager_H_

#include <SketchSolver_Group.h>

#include <Events_Listener.h>

#include <list>
#include <set>

class GeomAPI_Pnt2d;
class GeomDataAPI_Point2D;
class ModelAPI_CompositeFeature;
class SketchPlugin_Constraint;

/** \class   SketchSolver_Manager
 *  \ingroup Plugins
 *  \brief   Listens the changes of SketchPlugin features and transforms the Constraint
 *           feature into the format understandable by sketch solver.
 *
 *  \remark This is a singleton.
 */
class SketchSolver_Manager : public Events_Listener
{
public:
  /** \brief Main method to create constraint manager
   *  \return pointer to the singleton
   */
  static SketchSolver_Manager* instance();

  /** \brief Implementation of Event Listener method
   *  \param[in] theMessage the data of the event
   */
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  /**
   * The solver needs all the updated objects are transfered in one group, not one by one.
   * This iscreases performance and avoids problems in resolve of only part of the made updates.
   */
  virtual bool groupMessages();

protected:
  SketchSolver_Manager();
  ~SketchSolver_Manager();

  /** \brief Adds or updates a constraint or an entity in the suitable group
   *  \param[in] theFeature sketch feature to be changed
   *  \return \c true if the feature changed successfully
   */
  bool updateFeature(const std::shared_ptr<SketchPlugin_Feature>& theFeature);

  /** \brief Updates the sketch and related constraints, if the sketch plane is changed
   *  \param[in] theSketch sketch to be updated
   *  \return \c true if the sketch plane is changed
   */
  bool updateSketch(const std::shared_ptr<ModelAPI_CompositeFeature>& theSketch);

  /** \brief Move feature
   *  \param[in] theMovedFeature dragged sketch feature
   *  \param[in] theFromPoint    original position of the feature
   *  \param[in] theToPoint      prefereble position of the feature (current position of the mouse)
   *  \return \c true if the feature has been changed successfully
   */
  bool moveFeature(const std::shared_ptr<SketchPlugin_Feature>& theMovedFeature,
                   const std::shared_ptr<GeomAPI_Pnt2d>& theFromPoint,
                   const std::shared_ptr<GeomAPI_Pnt2d>& theToPoint);

  /** \brief Move feature using its moved attribute
   *  \param[in] theMovedAttribute  dragged point (array of points) attribute of sketch feature
   *  \param[in] theMovedPointIndex index of dragged point in an array (-1 otherwise)
   *  \param[in] theFromPoint       original position of the moved point
   *  \param[in] theToPoint         prefereble position (current position of the mouse)
   *  \return \c true if the attribute owner has been changed successfully
   */
  bool moveAttribute(const std::shared_ptr<ModelAPI_Attribute>& theMovedAttribute,
                     const int theMovedPointIndex,
                     const std::shared_ptr<GeomAPI_Pnt2d>& theFromPoint,
                     const std::shared_ptr<GeomAPI_Pnt2d>& theToPoint);

  /** \brief Removes a constraint from the manager
   *  \param[in] theConstraint constraint to be removed
   *  \return \c true if the constraint removed successfully
   */
  bool removeConstraint(std::shared_ptr<SketchPlugin_Constraint> theConstraint);

  /** \brief Goes through the list of groups and solve the constraints
   *  \return \c true, if groups are resolved, and features should be updated 
   *  (send the Update event)
   */
  bool resolveConstraints();

private:
  /** \brief Searches group which interact with specified feature
   *  \param[in]  theFeature  object to be found
   *  \return Pointer to corresponding group or NULL if the group cannot be created.
   */
  SketchGroupPtr findGroup(std::shared_ptr<SketchPlugin_Feature> theFeature);
  /** \brief Searches group related to specified composite feature
   *  \param[in]  theSketch  sketch to be found
   *  \return Pointer to corresponding group or NULL if the group cannot be created.
   */
  SketchGroupPtr findGroup(std::shared_ptr<ModelAPI_CompositeFeature> theSketch);

  /// \brief Stop sending the Update event until all features updated
  /// \return \c true, if the last flushed event is Update
  bool stopSendUpdate() const;
  /// \brief Allow to send the Update event
  void allowSendUpdate() const;

  /// \brief Allow send events about changing features in groups
  void releaseFeaturesIfEventsBlocked() const;

private:
  std::list<SketchGroupPtr> myGroups; ///< Groups of constraints
  /// true if computation is performed and all "updates" are generated by this algo
  /// and needs no recomputation
  bool myIsComputed;
};

#endif
