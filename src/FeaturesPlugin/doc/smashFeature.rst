
Smash
=====

The feature Smash implements a boolean operation for smashing a set of main objects by a set of tool objects.

To a create boolean opration Smash in the active part:

#. select in the Main Menu *Features - > Smash* item  or
#. click **Smash** button in the toolbar

.. image:: images/bool_smash.png
   :align: center

.. centered::
   **Smash**  button 

The following property panel will be opened:

.. image:: images/Smash.png
  :align: center

.. centered::
  **Smash operation**

Here it is necessary to select main objects and tool objects.

**Apply** button creates the smash shape.
  
**Cancel** button cancels operation.

**TUI Command**:  *model.addSmash(Part_doc, mainObjects, toolObjects)*

**Arguments**:   Part + list of main objects + list of tool objects.

Result
""""""

The Result of the operation will be a shape which is a smash of tool objects from main objects:

.. image:: images/CreatedSmash.png
	   :align: center

.. centered::
   **Smash created**

**See Also** a sample TUI Script of a :ref:`tui_create_smash` operation.
