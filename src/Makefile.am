AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -Wall -std=c++17 -DFORTIFY_SOURCE=2 -DGCRYPT_NO_DEPRECATED
AM_LDFLAGS = -Wl,-rpath=/lib/homegear -Wl,-rpath=/usr/lib/homegear -Wl,-rpath=/usr/local/lib/homegear

libdir = $(localstatedir)/lib/homegear/modules
lib_LTLIBRARIES = mod_homematicwired.la
mod_homematicwired_la_SOURCES = HMWired.h HMWiredPacket.h Factory.cpp GD.h HMWiredPacketManager.cpp HMWiredCentral.h HMWiredCentral.cpp HMWiredPeer.h HMWiredPacketManager.h GD.cpp Factory.h HMWiredPacket.cpp PhysicalInterfaces/IHMWiredInterface.cpp PhysicalInterfaces/HMW-LGW.cpp PhysicalInterfaces/IHMWiredInterface.h PhysicalInterfaces/RS485.h PhysicalInterfaces/HMW-LGW.h PhysicalInterfaces/RS485.cpp HMWired.cpp HMWiredDeviceTypes.h HMWiredPeer.cpp Interfaces.cpp Interfaces.h
mod_homematicwired_la_LDFLAGS =-module -avoid-version -shared
install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/mod_homematicwired.la
