.. _buildPolyline:

Polyline
========

The feature **Polyline** creates a wire.

To create a Polyline in the active part:

#. select in the Main Menu *Build - > Polyline* item  or
#. click **Polyline** button in the toolbar

.. image:: images/feature_polyline.png
   :align: center

.. centered::
   **Polyline** button 

The following property panel appears.

.. image:: images/Polyline.png
  :align: center

.. centered::
  Polyline property panel

Input fields:

- **Points and vertices** panel contains list of points and vertices selected in 3D OCC viewer. Polyline consequently connects points;
- **Closed** check-box turns on/off connection of the start and end points to make polyline closed/opened.

**TUI Commands**:  *model.addPolyline3D(Part_1_doc, Polyline_objects, Key)*

**Arguments**: 1 part+ list of vertices selected in format *model.selection("VERTEX", "Point")* + boolean key defining presence of connection between the start and end points.

Result
""""""
Result of operation is closed or open  Polyline.

.. image:: images/CreatePolyline.png
   :align: center

.. centered::
   Polyline

**See Also** a sample TUI Script of a :ref:`tui_create_polyline` operation.