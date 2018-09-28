.. _sketchLine:

Line
====

The feature Line creates a line segment by two points in the current Sketch.

To add new Line to the Sketch:

#. select in the Main Menu *Sketch - > Line* item  or
#. click **Line** button in Sketch toolbar:

.. image:: images/line.png
   :align: center

.. centered::
   **Line**  button

The following property panel appears.

.. image:: images/Line_panel.png
  :align: center

.. centered::
   Line

Pick 2 points (start and end) in the view to create a Line. In the case of click on an existing object constraint for
the point will be created automatically.

Start and end points coordinates are displayed in the property panel.

**TUI Command**:  *Sketch_1.addLine(X1, Y1, X2, Y2)*

**Arguments**:    4 values (coordinates of the start and end points).

Result
""""""

Created line appears in the view.

.. image:: images/Line_res.png
	   :align: center

.. centered::
   Line created

**See Also** a sample TUI Script of a :ref:`tui_create_line` operation.
