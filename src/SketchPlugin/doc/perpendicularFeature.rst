
Perpendicular constraint
========================

Perpendicular constraint fixes two lines to lie at 90 degrees to one another.

To create Perpendicular in the active Sketch:

#. select in the Main Menu *Sketch - > Perpendicular* item  or
#. click **Perpendicular** button in Sketch toolbar:

.. image:: images/perpendicular.png
   :align: center

.. centered::
   **Perpendicular**  button

Property panel:

.. image:: images/Perpendicular_panel.png
   :align: center

Input fields:

- **First line** is the first line selected in the view.
- **Second line** is the second line selected in the view.

After the lines are selected the special sign will be added to each of them in the view.

**TUI Command**: *Sketch_1.setPerpendicular(Line1, Line2)*

**Arguments**:  2 line objects

Result
""""""

Created Perpendicular constraint appears in the view.

.. image:: images/Perpendicular_res.png
	   :align: center

.. centered::
   Perpendicular constraint created

**See Also** a sample TUI Script of a :ref:`tui_create_perpendicular` operation.
