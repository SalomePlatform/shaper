.. |paper_roll.icon|    image:: images/paper_roll.png

.. _parameters:

Parameters manager
==================

To create/edit simultaneously a set of parameters  in the active partset or part instead of calling them one by one:

#. select in the Main Menu *Part - > Parameters* item or
#. click |paper_roll.icon| **Parameters** button in the toolbar:

The following dialog box with parameter table appears:
   
.. figure:: images/parameters.png
   :align: center

   Parameters dialog box

.. |param_up| image:: images/parameters_up.png
.. |param_down| image:: images/parameters_down.png

To edit a parameter, double-click on parameter in object browser.

**Input fields**:

- **Name** defines parameter name. Name follows the naming rules of the python language for variables;
- **Expression** allows manual input of python expression; 
- **Result** evaluates expression as real number by python *eval()* function;
- **Comment** contains any text. This field is optional;
- **Add** button adds a new empty string in the end of table. Default  **Name** is *<NoName>*, **Expression** is   *<NoValue>* 
- **Insert** button adds a new empty string before the selected parameter;
- **Delete** button removes the selected parameter from the table;   
- **Import** button import parameters from .txt files. Parameters must be written in file in separate lines like example: **Name** **Expression** #**Comment**
Sample.txt:
''Longueur 36. # Longueur de la pièce''
''Largeur 24 #Largeur de la pièce''
''Hauteur Longueur * Largeur''
- **Modify parameter position** button moves the selected parameter
  
    | |param_up| one string higher in the table,
    | |param_down| one string lower in the table.

**TUI Command**:

.. py:function:: model.importParameters(Part_doc, filename)

    :param part: The current part object
    :param filename: The txt file with parameters
    :return: Created objects.

**See Also** a sample TUI Script of a :ref:`tui_parameterFeature` operation.
**See Also** :ref:`parameter` operation.
