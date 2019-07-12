.. _parameters:
.. |paper_roll.icon|    image:: images/paper_roll.png


Parameters manager
==================

To create/edit simultaneously a set of parameters  in the active partset or part instead of calling them one by one:

#. select in the Main Menu *Part - > Parameters* item or
#. click |paper_roll.icon| **Parameters** button in the toolbar:

The following dialog box with parameter table appears:
   
.. image:: images/parameters.png
   :align: center

.. centered::
   Parameters dialog box

**Input fields**:

- **Name** defines parameter name. Name follows the naming rules of the python language for variables;
- **Expression** allows manual input of python expression; 
- **Result** evaluates expression as real number by python *eval()* function;
- **Comment** contains any text. This field is optional;
- **Add** button adds a new empty string in the end of table. Default  **Name** is *<NoName>*, **Expression** is   *<NoValue>* 
- **Insert** button adds a new empty string before the selected parameter;
- **Delete** button removes the selected parameter from the table;   
- **Modify parameter position** button moves the selected parameter
    .. image:: images/parameters_up.png
      :align: left
    one string higher in the table,
  
    .. image:: images/parameters_down.png
      :align: left
    one string lower in the table.

  
**TUI Command**: 

.. py:function:: model.addParameter(Part_doc, Name, Value, Comment)

    :param part: The current part object
    :param string: The parameter name
    :param number: The parameter value
    :param string: The parameter comment
    :return: Created object.

**See Also** a sample TUI Script of :ref:`tui_parametersFeature` operation.

Result
------

Created parameters and their current values are displayed in the Object Browser, in a special section **Parameters**. 

.. image:: images/object_browser_parameters.png
   :align: center

.. centered::
   **Parameters** in object browser
