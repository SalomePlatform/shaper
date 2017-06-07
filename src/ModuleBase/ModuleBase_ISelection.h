// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef ModuleBase_ISelection_H
#define ModuleBase_ISelection_H

#include "ModuleBase.h"
#include "ModuleBase_Definitions.h"

#include <ModelAPI_Result.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Vertex.h>

#include <QModelIndexList>
#include <AIS_ListOfInteractive.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>

#include <QList>

class ModuleBase_ViewerPrs;
class SelectMgr_EntityOwner;

/**
* \ingroup GUI
* A class which provides access to selection.
* A selection can be obtained as from a viewer as from Object browser in various forms
*/
class ModuleBase_ISelection
{
 public:
   /// Types of the selection place, where the selection is obtained
   enum SelectionPlace { Browser, Viewer, AllControls };

  /// Returns a list of viewer selected presentations
  /// \return list of presentations
  virtual QList<std::shared_ptr<ModuleBase_ViewerPrs>>
    getSelected(const SelectionPlace& thePlace = Browser) const = 0;

  /// The values are appended to the first parameter list if the first list does not contain
  /// an item with the same object
  /// \param theValues a list of new values
  /// \param theValuesTo a list, that will be changed
  static void appendSelected(const QList<std::shared_ptr<ModuleBase_ViewerPrs>> theValues,
                             QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValuesTo);

  /// Returns a list of viewer highlited presentations
  /// \return list of presentations
  virtual QList<std::shared_ptr<ModuleBase_ViewerPrs>> getHighlighted() const = 0;

  /// Fills the viewer presentation parameters by the parameters from the owner
  /// \param thePrs a container for selection
  /// \param theOwner a selection owner
  virtual void fillPresentation(std::shared_ptr<ModuleBase_ViewerPrs>& thePrs,
                                const Handle(SelectMgr_EntityOwner)& theOwner) const = 0;

  /**
   * Returns list of features currently selected in object browser
   */
  virtual QObjectPtrList selectedObjects() const = 0;

  /// Set selected objects list
  virtual void setSelectedObjects( const QObjectPtrList& ) const = 0;

  /**
   * Returns list of currently selected results
   */
  virtual QObjectPtrList selectedPresentations() const = 0;

  //! Returns list of currently selected QModelIndexes
  virtual QModelIndexList selectedIndexes() const = 0;

  //! Returns list of currently selected AIS objects
  //! \param theList returning list of selected AIS objects
  virtual void selectedAISObjects(AIS_ListOfInteractive& theList) const = 0;

  //! Returns list of currently selected QModelIndexes
  virtual ObjectPtr getSelectableObject(const Handle(SelectMgr_EntityOwner)& theOwner) const = 0;

  //! Return the shape from the viewer presentation.
  //! If the shape is equal to the shape of selected object, it returns an empty shape
  //! \param thePrs a selected object
  //! \return a shape
  MODULEBASE_EXPORT ResultPtr getResult(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  //! Return the shape from the viewer presentation.
  //! If the shape is equal to the shape of selected object, it returns an empty shape
  //! \param thePrs a selected object
  //! \return a shape
  MODULEBASE_EXPORT GeomShapePtr getShape(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  //! Return the IO from the viewer presentation.
  //! \param thePrs a selected object
  //! \return an interactive object
  virtual MODULEBASE_EXPORT
    Handle(AIS_InteractiveObject) getIO(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs) = 0;

  //! Wraps the object list into the viewer prs list
  //! \param theObjects a list of objects
  //! \return a list of prs, where only object is not empty
  static MODULEBASE_EXPORT QList<std::shared_ptr<ModuleBase_ViewerPrs>> getViewerPrs(
                                                       const QObjectPtrList& theObjects);

  /// Removes selection items where owners have equal vertices. The first
  /// owner with the qual vertex stays in the list.
  static MODULEBASE_EXPORT void filterSelectionOnEqualPoints
                                      (QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theSelected);
private:
  /// Find vertex shape build by a Brep owner of the presentation if it exists
  /// \param thePrs a viewer presentation
  /// \return GeomAPI wrap of vertex
  static std::shared_ptr<GeomAPI_Vertex> getPresentationVertex(
                                               const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);
};

#endif
