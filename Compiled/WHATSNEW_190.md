
## What is new in 1.9.0

 - **Unloaded drivers view**

<img src="https://raw.githubusercontent.com/hfiref0x/WinObjEx64/dev190/Screenshots/UnloadedDrivers.png" width="600" />

View contents of MmUnloadedDrivers ntoskrnl managed array.

 - **ALPC Ports connections**

<img src="https://raw.githubusercontent.com/hfiref0x/WinObjEx64/dev190/Screenshots/AlpcPortConnections.png" width="600" />

Display information about ALPC port connected clients, similar to WinDBG *!alpc /port* command.

 - **Section Object dump view**

<img src="https://raw.githubusercontent.com/hfiref0x/WinObjEx64/dev190/Screenshots/ViewingSectionObject.png" width="600" />

View contents of Section object type and it child structures such as CONTROL_AREA, SEGMENT etc. Note that view is not complete and only list important fields, similar to WinDBG *!ca 0xX 4* command.


 - **Other**
   + Added ExCreateCallback list to the callbacks list
   + Fixed several bugs
