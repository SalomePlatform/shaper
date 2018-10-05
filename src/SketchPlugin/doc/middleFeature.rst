
Middle point constraint
=======================

Middle point constraint forces a point to middle of a line.

To create a Middle point in the active Sketch:

#. select in the Main Menu *Sketch - > Middle point* item  or
#. click **Middle point** button in Sketch toolbar:

.. image:: images/middlepoint.png
   :align: center

.. centered::
   **Middle point**  button

Property panel:

.. image:: images/Middlepoint_panel.png
   :align: center

Input fields:

- **First object** is a point or a line selected in the view.
- **Second object** is a point or a line selected in the view.

Note that one of two objects should be a line and the other a point (i.e. a point, a line or an arc end point, a center of a circle or an arc).

| After the objects are selected, the point is moved to the middle of the line.
| The middle points are marked with a special sign.

**TUI Command**: *Sketch_1.setMiddlePoint(Point, Line)*

**Arguments**:  2 objects (the first is a point, the second one is a line)

Result
""""""

Created Middle point constraint appears in the view.

.. image:: images/Middlepoint_res.png
	   :align: center

.. centered::
   Created middle point constraint

**See Also** a sample TUI Script of :ref:`tui_create_middle` operation.
