.. _featureCut:

Cut
===

The feature Cut implements a boolean operation to cut tool objects from a main objects.

To perform a boolean operation Cut in the active part:

#. select in the Main Menu *Features - > Cut* item  or
#. click **Cut** button in the toolbar

.. image:: images/bool_cut.png
   :align: center

.. centered::
   **Cut**  button 

The following property panel will be opened:

.. image:: images/boolean_cut_property_panel.png
  :align: center

.. centered::
  **Cut operation**

- **Main Objects** contains a list of objects selected in the Object Browser or in the Viewer, which will be cut by tool objects.
  Any kind of shape can be selected. Also allows selection of compsolids/compound subshapes.
  In this case only selected subshapes will be cut, other will stay untoched (as much as possible).
- **Tool Objects** contains a list of objects selected in the Object Browser or in the Viewer, which will be cut main objects.
  Any kind of shape can be selected. Also allows selection of compsolids/compounds subshapes.
  Non-selected subshapes from compsolids/compounds will be ignored.
- **See preview** button shows a result of the operation.

**TUI Command**:  *model.addCut(Part_doc, mainObjects, toolObjects)*

**Arguments**:   Part + list of main objects + list of tool objects.

Result
""""""

For each selected object will be produced a result shape which is a cut of tool objects from main object:

.. image:: images/CreatedCut.png
   :align: center

.. centered::
   **Cut created**

In case if subsolid (yellow wireframe) of compsolid was selected:

.. image:: images/boolean_cut_subsolids_arguments.png
   :align: center

.. centered::
   **Cut arguments**

then it will be cut in result and not selected subsolids will be added to it:

.. image:: images/boolean_cut_subsolids_result.png
   :align: center

.. centered::
   **Cut subsolid result**


**See Also** a sample TUI Script of a :ref:`tui_create_cut` operation.
