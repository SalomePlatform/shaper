
Vertical constraint
===================

Vertical constraint fixes lines as parallel to the vertical axis.

To create a Vertical constraint in the active Sketch:

#. select in the Main Menu *Sketch - > Vertical* item  or
#. click **Vertical** button in Sketch toolbar:

.. image:: images/vertical.png
   :align: center

.. centered::
   **Vertical**  button

Property panel:

.. image:: images/Vertical_panel.png
   :align: center

Input fields:

- **Line** is the line selected in the view.

| After the line is selected it becomes parallel to the vertical axis in the view.
| **V** sign is added above the line.

**TUI Command**: *Sketch_1.setVertical(LineObject)*

**Arguments**:  1 line object

Result
""""""

Created Vertical constraint appears in the view.

.. image:: images/Vertical_res.png
	   :align: center

.. centered::
   Vertical constraint created

**See Also** a sample TUI Script of :ref:`tui_create_vertical` operation.