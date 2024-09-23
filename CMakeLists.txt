cmake_minimum_required(VERSION 3.8)
project(homegear_homematicwired)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
        src/PhysicalInterfaces/HMW-LGW.cpp
        src/PhysicalInterfaces/HMW-LGW.h
        src/PhysicalInterfaces/IHMWiredInterface.cpp
        src/PhysicalInterfaces/IHMWiredInterface.h
        src/PhysicalInterfaces/RS485.cpp
        src/PhysicalInterfaces/RS485.h
        src/Factory.cpp
        src/Factory.h
        src/GD.cpp
        src/GD.h
        src/HMWired.cpp
        src/HMWired.h
        src/HMWiredCentral.cpp
        src/HMWiredCentral.h
        src/HMWiredPacket.cpp
        src/HMWiredPacket.h
        src/HMWiredPacketManager.cpp
        src/HMWiredPacketManager.h
        src/HMWiredPeer.cpp
        src/HMWiredPeer.h
        src/Interfaces.cpp
        src/Interfaces.h)

add_custom_target(homegear COMMAND ../../makeAll.sh SOURCES ${SOURCE_FILES})

add_library(homegear_homematicwired ${SOURCE_FILES})