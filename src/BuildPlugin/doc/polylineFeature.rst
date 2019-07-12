.. _buildPolyline:
.. |feature_polyline.icon|    image:: images/feature_polyline.png

Polyline
========

Polyline feature creates a wire.

To create a Polyline in the active part:

#. select in the Main Menu *Build - > Polyline* item  or
#. click |feature_polyline.icon| **Polyline** button in the toolbar

The following property panel appears.

.. image:: images/Polyline.png
  :align: center

.. centered::
  Polyline property panel

Input fields:

- **Points and vertices** panel contains a list of points and vertices selected in 3D OCC viewer. The Polyline consequently connects points;
- **Closed** check-box turns on/off connection of the start and end points to make polyline closed/opened.

**TUI Commands**:

.. py:function:: model.addPolyline3D(Part_1_doc, Polyline_objects, Key)

    :param part: The current part object.
    :param list: list of vertices selected in format *model.selection("VERTEX", "Point")*.
    :param boolean: Is closed.
    :return: Result object.

Result
""""""
The result of the operation is a closed or open Polyline.

.. image:: images/CreatePolyline.png
   :align: center

.. centered::
   Polyline

**See Also** a sample TUI Script of :ref:`tui_create_polyline` operation.