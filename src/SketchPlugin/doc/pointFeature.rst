
Point
=====

Point feature creates a point in the current Sketch.

To add a new Point to the Sketch:

#. select in the Main Menu *Sketch - > Point* item  or
#. click **Point** button in Sketch toolbar:

.. image:: images/point.png
   :align: center

.. centered::
   **Point**  button

The following property panel appears:

.. image:: images/Point_panel.png
  :align: center

.. centered::
   Point

Clicking in the view creates a point preview. If an existing object is clicked, a constraint for
the point will be created automatically.

The point coordinates are displayed in the property panel.

**TUI Command**:  *Sketch_1.addPoint(X, Y)*

**Arguments**:    2 values (coordinates on the Sketch plane).

Result
""""""

Created point appears in the view.

.. image:: images/Point_res.png
	   :align: center

.. centered::
   Point created

**See Also** a sample TUI Script of :ref:`tui_create_point` operation.