# eaton_hw

The device sends message to monitor program.
Each message has an unique identifier - a 24 bit long integer, a data - 32 bit long integer, an LRC - 8 bit long integer.

Message structure:

{ | 8bit - LRC |         32 bit - data         |   24 bit - id   | }

Monitor can get messages by receivers or by adding directly.

Receivers can be added to the system one by one or bundle.

Monitor checks if received message is valid. Furthermore, It counts all types of messages (valid, invalid).

If message is INVALID then you can find "#Damaged data, device id: " line in console. Otherwise, It will write all information about message ( id, LRC, data, full message as integer).

//-------------------------------------------------------------------------------------------------

You can modify number of messages in file monitor.cpp in main function by changing parameters in DeviceMock::MessageSender.