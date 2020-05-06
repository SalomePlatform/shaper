.. |feature_interpolation.icon|    image:: images/feature_interpolation.png

Interpolation
=============

Interpolation feature creates a curve (edge) using vertices already existing in other objects.

To create an interpolation in the active part:

#. select in the Main Menu *Build - > Interpolation* item  or
#. click |feature_interpolation.icon| **Interpolation** button in the toolbar

The following property panel will be opened:

.. figure:: images/Interpolation.png
  :align: center

  Create an interpolation

Select one or several vertices or points in the viewer.

- **Closed** makes the interpolation curve closed.

- **Reorder** reorders the selected points to minimize the length of curve.

- **Tangents** makes start and end of the curve tangent to the selected edges. The user has to select start and end edges in the corresponding fields.

**Apply** button creates an interpolation.

**Cancel** button cancels the operation. 

**TUI Commands**:

.. py:function:: model.addInterpolation(Part_doc, Points, IsClosed, IsReordered)

    :param part: The current part object.
    :param list: A list of points.
    :param boolean: Is closed.
    :param boolean: Is reordered.
    :return: Result object.

.. py:function:: model.addInterpolation(Part_doc, Points, StartEdge, EndEdge, IsClosed, IsReordered)

    :param part: The current part object.
    :param list: A list of points.
    :param object: Start point.
    :param object: End point.
    :param boolean: Is closed.
    :param boolean: Is reordered.
    :return: Result object.

Result
""""""

The result of the operation will be a curve created from the selected shapes:

.. figure:: images/CreateInterpolation.png
  :align: center

  Result of the operation.

**See Also** a sample TUI Script of :ref:`tui_create_interpolation` operation.
