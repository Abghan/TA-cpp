Requirements
1. Install paho.mqtt.c
    with ubuntu:
    $ git clone https://github.com/eclipse/paho.mqtt.c.git
    $ cd paho.mqtt.c
    $ git checkout v1.3.8

    $ cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON \
        -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON
    $ sudo cmake --build build/ --target install
    $ sudo ldconfig
2. Install paho.mqtt.cpp
    with ubuntu:
    $ git clone https://github.com/eclipse/paho.mqtt.cpp
    $ cd paho.mqtt.cpp

    $ cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON \
        -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE
    $ sudo cmake --build build/ --target install
    $ sudo ldconfig
3. Install jsoncpp