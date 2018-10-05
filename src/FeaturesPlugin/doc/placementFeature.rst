
Placement
=========

Placement defines position of an object relatively to another object. To make a placement:

#. select in the Main Menu *Part - > Placement* item  or
#. click **Placement** button in the toolbar

.. image:: images/placement_btn.png
   :align: center

.. centered::
   **Placement**  button 

The following property panel will be opened:

.. image:: images/Placement.png
  :align: center

.. centered::
   **Placement operation**

In this property panel, it is necessary to:

-  Select objects which will be moved;

-  Select a face, edge or vertex as a start for moving;

-  Select a face, edge or vertex as an end of moving;

-  Define state of **Reverse** and **Centering** check boxes.



**Apply** button creates the placement.
  
**Cancel** button cancels the operation.

**TUI Command**:  *model.addPlacement(Part_doc, placeObjects, startShape, endShape, isReverse, isCentering)*

**Arguments**:   Part + list of objects to move + start shape + end shape + is reverse flag + is centering flag.

Result
""""""

The Result of the operation will be a new placement of selected objects:

.. image:: images/CreatedPlacement.png
	   :align: center

.. centered::
   **Placement created**

**See Also** a sample TUI Script of :ref:`tui_create_placement` operation.
