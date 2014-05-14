// File:        PartSet_OperationSketchBase.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationSketchBase_H
#define PartSet_OperationSketchBase_H

#include "PartSet.h"

#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_List.hxx>

#include <ModuleBase_Operation.h>
#include <ModuleBase_Operation.h>
#include <QObject>

class Handle_V3d_View;
class QMouseEvent;
class GeomAPI_Shape;
class XGUI_ViewerPrs;

/*!
  \class PartSet_OperationSketchBase
  * \brief The base operation for the sketch features.
  *  Base class for all sketch operations. It provides an access to the feature preview
*/
class PARTSET_EXPORT PartSet_OperationSketchBase : public ModuleBase_Operation
{
  Q_OBJECT
public:
  enum FeatureActionMode { FM_Activation, FM_Deactivation, FM_Abort };

public:
  /// Constructor
  /// \param theId an feature index
  /// \param theParent the object parent
  PartSet_OperationSketchBase(const QString& theId, QObject* theParent);
  /// Destructor
  virtual ~PartSet_OperationSketchBase();

  /// Returns the feature preview shape
  /// \param theFeature the feature object to obtain the preview
  boost::shared_ptr<GeomAPI_Shape> preview(boost::shared_ptr<ModelAPI_Feature> theFeature) const;

  /// Returns the operation local selection mode
  /// \param theFeature the feature object to get the selection mode
  /// \return the selection mode
  virtual std::list<int> getSelectionModes(boost::shared_ptr<ModelAPI_Feature> theFeature) const = 0;

  /// Initializes some fields accorging to the feature
  /// \param theFeature the feature
  virtual void init(boost::shared_ptr<ModelAPI_Feature> theFeature) {}

  /// Processes the mouse pressed in the point
  /// \param thePoint a point clicked in the viewer
  /// \param theEvent the mouse event
  virtual void mousePressed(QMouseEvent* theEvent, Handle_V3d_View theView);

  /// Processes the mouse release in the point
  /// \param thePoint a point clicked in the viewer
  /// \param theEvent the mouse event
  /// \param theSelected the list of selected presentations
  virtual void mouseReleased(QMouseEvent* theEvent, Handle_V3d_View theView,
                             const std::list<XGUI_ViewerPrs>& theSelected);

  /// Processes the mouse move in the point
  /// \param thePoint a 3D point clicked in the viewer
  /// \param theEvent the mouse event
  /// \param theSelected the list of selected presentations
  virtual void mouseMoved(QMouseEvent* theEvent, Handle_V3d_View theView,
                          const std::list<XGUI_ViewerPrs>& theSelected);

  /// Processes the key pressed in the view
  /// \param theKey a key value
  virtual void keyReleased(const int theKey) {};

signals:
  /// Signal about the feature construing is finished
  /// \param theFeature the result feature
  /// \param theMode the mode of the feature modification
  void featureConstructed(boost::shared_ptr<ModelAPI_Feature> theFeature,
                          int theMode);
  /// signal about the request to launch operation
  /// theName the operation name
  /// theFeature the operation argument
  void launchOperation(std::string theName, boost::shared_ptr<ModelAPI_Feature> theFeature);
  /// signal to enable/disable multi selection in the viewer
  /// \param theEnabled the boolean state
  void multiSelectionEnabled(bool theEnabled);

  /// signal to enable/disable usual selection in the viewer
  /// \param theEnabled the boolean state
  void selectionEnabled(bool theEnabled);

protected:
  /// Creates an operation new feature
  /// In addition to the default realization it appends the created line feature to
  /// the sketch feature
  /// \returns the created feature
  virtual boost::shared_ptr<ModelAPI_Feature> createFeature();
};

#endif
