// File:        PartSet_OperationFeatureEditMulti.h
// Created:     05 May 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationFeatureEditMulti_H
#define PartSet_OperationFeatureEditMulti_H

#include "PartSet.h"

#include <PartSet_OperationSketchBase.h>
#include <QObject>

class QMouseEvent;

/*!
 \class PartSet_OperationFeatureEditMulti
 * \brief The operation for the sketch feature creation
 */
class PARTSET_EXPORT PartSet_OperationFeatureEditMulti : public PartSet_OperationSketchBase
{
Q_OBJECT
  /// Struct to define gp point, with the state is the point is initialized
  struct Point
  {
    /// Constructor
    Point()
    {
    }
    /// Constructor
    /// \param thePoint the point
    Point(gp_Pnt thePoint)
    {
      setPoint(thePoint);
    }
    ~Point()
    {
    }

    /// clear the initialized flag.
    void clear()
    {
      myIsInitialized = false;
    }
    /// set the point and switch on the initialized flag
    /// \param thePoint the point
    void setPoint(const gp_Pnt& thePoint)
    {
      myIsInitialized = true;
      myPoint = thePoint;
    }

    bool myIsInitialized;  /// the state whether the point is set
    gp_Pnt myPoint;  /// the point
  };

 public:
  /// Returns the operation type key
  static std::string Type()
  {
    return "EditMulti";
  }

 public:
  /// Constructor
  /// \param theId the feature identifier
  /// \param theParent the operation parent
  /// \param theFeature the parent feature
  PartSet_OperationFeatureEditMulti(const QString& theId, QObject* theParent,
                                    CompositeFeaturePtr theFeature);
  /// Destructor
  virtual ~PartSet_OperationFeatureEditMulti();

  /// Initialisation of operation with preliminary selection
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void initSelection(const std::list<ModuleBase_ViewerPrs>& theSelected,
                             const std::list<ModuleBase_ViewerPrs>& theHighlighted);

  /// Returns the operation sketch feature
  /// \returns the sketch instance
  virtual CompositeFeaturePtr sketch() const;

  /// Processes the mouse pressed in the point
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void mousePressed(QMouseEvent* theEvent, Handle_V3d_View theView,
                            const std::list<ModuleBase_ViewerPrs>& theSelected,
                            const std::list<ModuleBase_ViewerPrs>& theHighlighted);
  /// Gives the current mouse point in the viewer
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  virtual void mouseMoved(QMouseEvent* theEvent, Handle_V3d_View theView);
  /// Gives the current selected objects to be processed by the operation
  /// \param thePoint a point clicked in the viewer
  /// \param theEvent the mouse event
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void mouseReleased(QMouseEvent* theEvent, Handle_V3d_View theView,
                             const std::list<ModuleBase_ViewerPrs>& theSelected,
                             const std::list<ModuleBase_ViewerPrs>& theHighlighted);
 protected:
  /// \brief Virtual method called when operation is started
  /// Virtual method called when operation started (see start() method for more description)
  /// Switch off the multi selection state
  virtual void startOperation();

  /// Virtual method called when operation stopped - committed or aborted.
  /// Restore the multi selection state
  virtual void stopOperation();

 protected:
  /// Emits a signal about the selection blocking. Emits a signal to change the selection.
  /// If the block is true, the signal clear selection, otherwise if restore selection flag allows,
  /// the internal operation features are to be selected
  /// \param isBlocked the state whether the operation is blocked or unblocked
  /// \param isRestoreSelection the state whether the selected objects should be reselected
  void blockSelection(bool isBlocked, const bool isRestoreSelection = true);

  /// Sends the features
  void sendFeatures();

 private:
  CompositeFeaturePtr mySketch;  ///< the sketch feature
  std::list<ModuleBase_ViewerPrs> myFeatures;  ///< the features to apply the edit operation
  Point myCurPoint;  ///< the current 3D point clicked or moved
  bool myIsBlockedSelection;  ///< the state of the last state of selection blocked signal
};

#endif
