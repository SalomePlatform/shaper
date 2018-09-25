
Horizontal constraint
=====================

Horizontal constraint fixes lines to lie parallel to the horizontal axis.

To create Horizontal constraint in the active Sketch:

#. select in the Main Menu *Sketch - > Horizontal* item  or
#. click **Horizontal** button in Sketch toolbar:

.. image:: images/horisontal.png
   :align: center

.. centered::
   **Horizontal**  button

Property panel:

.. image:: images/Horizontal_panel.png
   :align: center

Input fields:

- **Line** is the line selected in the view.

| After the line is selected it becomes parallel to the horizontal axis in the view.
| **H** sign added above the line.

**TUI Command**: *Sketch_1.setHorizontal(LineObject)*

**Arguments**:  1 line object

Result
""""""

Created Horizontal constraint appears in the view.

.. image:: images/Horizontal_res.png
	   :align: center

.. centered::
   Horizontal constraint created

**See Also** a sample TUI Script of a :ref:`tui_create_horizontal` operation.