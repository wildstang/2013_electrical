WildStang Signs
===============

Introduction:
-------------

This is the WildStang Signs project. Our implementation calls for 3 signs with
52 LEDs each in the shape of a 1 as defined physical design section. The actual
design of the signs themselves can be altered to match other numbers as desired.
Please note though that the code will have to be altered to match the new
orientation.

Notes:
------

⁃	delay() cannot be used as it completely halts all operations. Thus, I2C
packets will not be handled during a delay() period. Use the timedWaitFunction.

Todo:
-----

-   Integrate accelerometer code into control box

-   Optimize existing code

-   Correct formatting variations throughout code
