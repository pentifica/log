A light-weight fast logging framework. Based on the concept of first capturing critical information in raw form.
Then recording the captured critical information in a convenient format at a later time.

# build
cd *<root directory of download>*
mkdir build
cd build
cmake ..
make

# install
cd *<root directory of download>*
cd build
make install

# usage
## Event
The base class for all information processed by the logging system. Derived classes must provide an implementation of the **Log** method. The **Log** method is called when an instance if **Event** is streamed.
## Generic Event
Derived from the **Event** class, this class can capture and stream arbitrary information as a tuple.

## Manager
The purpose of this class is to provide a delayed ordered streaming of event information.  It does this by first storing event information in a ring buffer. Then, at user determined intervals, stream some or all of the stored information.

## Factory
This static class is provided to reduce the overhead associated with repeatedly creating an event message. The templated **Factory** class allocates instances of an event which are re-used after the instance has been streamed.