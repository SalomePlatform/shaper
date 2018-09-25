
Collinear constraint
====================

Collinear constraint makes to lines collinear.

To create Collinear in the active Sketch:

#. select in the Main Menu *Sketch - > Collinear* item  or
#. click **Collinear** button in Sketch toolbar:

.. image:: images/collinear.png
   :align: center

.. centered::
   **Collinear**  button

Property panel:

.. image:: images/Collinear_panel.png
   :align: center

Input fields:

- **First line** is the first line selected in the view.
- **Second line** is the second line selected in the view.

| After the lines are selected they becomes translated as to be collinear.
| The lines are marked with the special sign.

**TUI Command**: *Sketch_1.setCollinear(Line1, Line2)*

**Arguments**:  2 line objects

Result
""""""

Created Collinear constraint appears in the view.

.. image:: images/Collinear_res.png
	   :align: center

.. centered::
   Collinear constraint created

**See Also** a sample TUI Script of a :ref:`tui_create_collinear` operation.
