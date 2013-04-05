WildStang Signs Protocol Discussion
===================================

Basic Packet Design:
--------------------

### Command Packet:

(light function) || (function variable 1) || (function variable 2) || … [repeats
up to a total of 8 bytes maximum]



Each section consists of 1 byte which are all sent in series. The actual size of
the whole "packet" varies depending on what each light function calls for. A
"-1" value is set within the storage array to signify the end of the packet if
the total length is under 8 bytes.

### Request Packet:

(current function) || (position in function 1) || (position in function 2)



Explanation 
------------

Our current design calls for all 3 signs to communicate with each other and the
control box over the I2C protocol. In addition, the control box communicates
with the screen via I2C. Since all data holds the same priority, checks must be
included in code to ensure that both the display and the signs are given an
equal chance to send their respective bundles of data. Wait periods should also
be added for functions that send data on every loop through to prevent
unnecessarily flooding the lines.

For a list of the available packets and their corresponding variables, see
packet.xls.
