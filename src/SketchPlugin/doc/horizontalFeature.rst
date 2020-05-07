.. |horisontal.icon|    image:: images/horisontal.png

Horizontal constraint
=====================

Horizontal constraint fixes lines as parallel to the horizontal axis.

To create a Horizontal constraint in the active Sketch:

#. select in the Main Menu *Sketch - > Horizontal* item  or
#. click |horisontal.icon| **Horizontal** button in Sketch toolbar:

Property panel:

.. figure:: images/Horizontal_panel.png
   :align: center

Input fields:

- **Line** is the line selected in the view.

| After the line is selected it becomes parallel to the horizontal axis in the view.
| **H** sign is added above the line.

**TUI Command**:

.. py:function:: Sketch_1.setHorizontal(LineObject)

    :param object: A line.
    :return: Result object.

Result
""""""

Created Horizontal constraint appears in the view.

.. figure:: images/Horizontal_res.png
   :align: center

   Horizontal constraint created

**See Also** a sample TUI Script of :ref:`tui_create_horizontal` operation.