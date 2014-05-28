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

#include <map>

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
  enum FeatureActionMode { FM_Activation, FM_Deactivation, FM_Hide };

public:
  /// Constructor
  /// \param theId an feature index
  /// \param theParent the object parent
  PartSet_OperationSketchBase(const QString& theId, QObject* theParent);
  /// Destructor
  virtual ~PartSet_OperationSketchBase();

  /// Returns the feature preview shape
  /// \param theFeature the feature object to obtain the preview
  static boost::shared_ptr<GeomAPI_Shape> preview(boost::shared_ptr<ModelAPI_Feature> theFeature);

  /// Returns the map of the operation previews including the nested feature previews
  /// \return the map of feature to the feature preview
  virtual std::map<boost::shared_ptr<ModelAPI_Feature>, boost::shared_ptr<GeomAPI_Shape> > subPreview() const;

  /// Returns the operation local selection mode
  /// \param theFeature the feature object to get the selection mode
  /// \return the selection mode
  virtual std::list<int> getSelectionModes(boost::shared_ptr<ModelAPI_Feature> theFeature) const = 0;

  /// Initializes some fields accorging to the feature
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void init(boost::shared_ptr<ModelAPI_Feature> theFeature,
                    const std::list<XGUI_ViewerPrs>& theSelected,
                    const std::list<XGUI_ViewerPrs>& theHighlighted) {}

  /// Returns the operation sketch feature
  /// \returns the sketch instance
  virtual boost::shared_ptr<ModelAPI_Feature> sketch() const = 0;

  /// Processes the mouse pressed in the point
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void mousePressed(QMouseEvent* theEvent, Handle_V3d_View theView,
                            const std::list<XGUI_ViewerPrs>& theSelected,
                            const std::list<XGUI_ViewerPrs>& theHighlighted);

  /// Processes the mouse release in the point
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void mouseReleased(QMouseEvent* theEvent, Handle_V3d_View theView,
                             const std::list<XGUI_ViewerPrs>& theSelected,
                             const std::list<XGUI_ViewerPrs>& theHighlighted);

  /// Processes the mouse move in the point
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  virtual void mouseMoved(QMouseEvent* theEvent, Handle_V3d_View theView);

  /// Processes the key pressed in the view
  /// \param theKey a key value
  virtual void keyReleased(const int theKey);

  virtual void keyReleased(std::string theName, QKeyEvent* theEvent);

signals:
  /// signal about the request to launch operation
  /// theName the operation name
  /// theFeature the operation argument
  void launchOperation(std::string theName, boost::shared_ptr<ModelAPI_Feature> theFeature);
  /// signal about the focus activated
  /// theName the attribute name
  void focusActivated(const std::string& theAttibuteName);

  /// Signal about the feature construing is finished
  /// \param theFeature the result feature
  /// \param theMode the mode of the feature modification
  void featureConstructed(boost::shared_ptr<ModelAPI_Feature> theFeature,
                          int theMode);
  /// Signal about the features should be selected
  /// \param theSelected the list of selected presentations
  void featureSelected(const std::list<XGUI_ViewerPrs>& theSelected);
  /// signal to enable/disable multi selection in the viewer
  /// \param theEnabled the boolean state
  void multiSelectionEnabled(bool theEnabled);

  /// signal to enable/disable selection in the viewer
  /// \param theFeatures a list of features to be disabled
  /// \param theToStop the boolean state whether it it stopped or non stopped
  void stopSelection(const std::list<XGUI_ViewerPrs>& theFeatures, const bool theToStop);
  /// signal to set selection in the viewer
  /// \param theFeatures a list of features to be disabled
  void setSelection(const std::list<XGUI_ViewerPrs>& theFeatures);

  /// signal to close the operation local context if it is opened
  void closeLocalContext();

protected:
  /// Creates an operation new feature
  /// In addition to the default realization it appends the created line feature to
  /// the sketch feature
  /// \param theFlushMessage the flag whether the create message should be flushed
  /// \returns the created feature
  virtual boost::shared_ptr<ModelAPI_Feature> createFeature(const bool theFlushMessage = true);
};

#endif
