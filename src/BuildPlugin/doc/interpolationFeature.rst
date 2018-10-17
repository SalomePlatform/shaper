
Interpolation
=============

Interpolation feature creates a curve (edge) using vertices already existing in other objects.

To create an interpolation in the active part:

#. select in the Main Menu *Build - > Interpolation* item  or
#. click **Interpolation** button in the toolbar

.. image:: images/feature_interpolation.png
  :align: center

.. centered::
  **Interpolation** button

The following property panel will be opened:

.. image:: images/Interpolation.png
  :align: center

.. centered::
  Create an interpolation

Select one or several vertices or points in the viewer.

- **Closed** makes the interpolation curve closed.

- **Reorder** reorders the selected points to minimize the length of curve.

- **Tangents** makes start and end of the curve tangent to the selected edges. The user has to select start and end edges in the corresponding fields.

**Apply** button creates an interpolation.

**Cancel** button cancels the operation. 

**TUI Command**:

- *model.addInterpolation(Part_doc, Points, IsClosed, IsReordered)*.

**Arguments**:   Part document + list of points + is closed + is reordered.

- *model.addInterpolation(Part_doc, Points, StartEdge, EndEdge, IsClosed, IsReordered)*.

**Arguments**:   Part document + list of points + start edge + end edge + is closed + is reordered.

Result
""""""

The result of the operation will be a curve created from the selected shapes:

.. image:: images/CreateInterpolation.png
  :align: center

.. centered::
  Result of the operation.

**See Also** a sample TUI Script of :ref:`tui_create_interpolation` operation.
