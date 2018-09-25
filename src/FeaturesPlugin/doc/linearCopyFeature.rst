
Linear copy
===========

The feature **Linear copy** makes several translations of a selected shape in one or two directions.

To create Linear copy in the active part:

#. select in the Main Menu *Part - > Linear copy* item  or
#. click **Linear copy** button in the toolbar

.. image:: images/multitranslation.png  
   :align: center

.. centered::
   **Linear copy** button 

The following property panel appears.

.. image:: images/LinearCopy.png
  :align: center

.. centered::
  Linear copy property panel

Input fields:

- **Main objects** panel contains shapes to be translated. Shapes are selected in 3D OCC viewer or object browser;
- **First direction** defines translation in the first direction:

  - **Vector of translation**  defines vector along which the object will be translated. Vector is edge, axis selected in 3D OCC viewer or object browser;
  - **Step** defines the distance between the shape copies;
  - **Nb. Copies** is the number of shape copies;
 
- **Second direction** check-box turns on/off translation in the second direction:

  - **Vector of translation**  defines vector along which the object will be translated. Vector is edge, axis selected in 3D OCC viewer or object browser;
  - **Step** defines the distance between the shape copies;
  - **Nb. Copies** defines the number of shape copies.

**TUI Command**:  *model.addMultiTranslation(Part_doc, [shape], axis1, step1, Nb1, axis2, step2, Nb2)*

**Arguments**: part + list of shapes in format *model.selection(TYPE, shape)* + 2 axis in format *model.selection(TYPE, shape)*+ 2 real (step values) + 2 integers (Nb. Copies).

Result
""""""
Result of operation is compound.

.. image:: images/linearcopy.png
   :align: center

.. centered::
   Linear copy

**See Also** a sample TUI Script of a :ref:`tui_create_linearcopy` operation.