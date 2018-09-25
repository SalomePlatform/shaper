  .. _create_sketch_rectangle:

Rectangle
=========

The feature Rectangle creates a rectangle by two opposite points in the current Sketch.

The result is represented in the form of four Lines which compose a rectangle.
Two of the lines have Vertical constraint and two others have Horizontal constraint.

To add new Rectangle to the Sketch:

#. select in the Main Menu *Sketch - > Rectangle* item  or
#. click **Rectangle** button in Sketch toolbar:

.. image:: images/rectangle.png
   :align: center

.. centered::
   **Rectangle**  button

The following property panel appears.

.. image:: images/Rectangle_panel.png
  :align: center

.. centered::
   Rectangle

Click in the view once to set the first corner, then move the mouse and click a second time to set the opposite corner.
In the case of click on an existing object constraint for the point will be created automatically.

Start and end points coordinates are displayed in the property panel.

**TUI Command**:  *Sketch_1.addRectangle(X1, Y1, X2, Y2)*

**Arguments**:    4 values (coordinates of the start and end points).

Result
""""""

Created rectangle appears in the view.

.. image:: images/Rectangle_res.png
	   :align: center

.. centered::
   Rectangle created

**See Also** a sample TUI Script of a :ref:`tui_create_rectangle` operation.