/* Copyright 2013-2016 Sathya Laufer
 *
 * Homegear is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Homegear is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Homegear.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#ifndef HMWIREDCENTRAL_H_
#define HMWIREDCENTRAL_H_

#include <homegear-base/BaseLib.h>
#include "HMWiredPeer.h"
#include "HMWiredPacketManager.h"

#include <memory>
#include <mutex>
#include <string>

namespace HMWired
{

class HMWiredCentral : public BaseLib::Systems::ICentral
{
public:
	HMWiredCentral(ICentralEventSink* eventHandler);
	HMWiredCentral(uint32_t deviceType, std::string serialNumber, int32_t address, ICentralEventSink* eventHandler);
	virtual ~HMWiredCentral();
	virtual void dispose(bool wait = true);

	std::shared_ptr<HMWiredPeer> getPeer(int32_t address);
	std::shared_ptr<HMWiredPeer> getPeer(uint64_t id);
	std::shared_ptr<HMWiredPeer> getPeer(std::string serialNumber);
	virtual void saveMessageCounters();
	virtual void serializeMessageCounters(std::vector<uint8_t>& encodedData);
	virtual void unserializeMessageCounters(std::shared_ptr<std::vector<char>> serializedData);
	virtual uint8_t getMessageCounter(int32_t destinationAddress);

	virtual bool isInPairingMode() { return _pairing; }
	virtual std::shared_ptr<HMWiredPacket> sendPacket(std::shared_ptr<HMWiredPacket> packet, bool resend, bool systemResponse = false);
	std::shared_ptr<HMWiredPacket> getSentPacket(int32_t address) { return _sentPackets.get(address); }

	virtual std::shared_ptr<HMWiredPacket> getResponse(uint8_t command, int32_t destinationAddress, bool synchronizationBit = false);
	virtual std::shared_ptr<HMWiredPacket> getResponse(std::vector<uint8_t>& payload, int32_t destinationAddress, bool synchronizationBit = false);
	virtual std::shared_ptr<HMWiredPacket> getResponse(std::shared_ptr<HMWiredPacket> packet, bool systemResponse = false);
	virtual std::vector<uint8_t> readEEPROM(int32_t deviceAddress, int32_t eepromAddress);
	virtual bool writeEEPROM(int32_t deviceAddress, int32_t eepromAddress, std::vector<uint8_t>& data);
	virtual void sendOK(int32_t messageCounter, int32_t destinationAddress);

	virtual bool onPacketReceived(std::string& senderID, std::shared_ptr<BaseLib::Systems::Packet> packet);
	std::string handleCliCommand(std::string command);
	uint64_t getPeerIdFromSerial(std::string& serialNumber) { std::shared_ptr<HMWiredPeer> peer = getPeer(serialNumber); if(peer) return peer->getID(); else return 0; }
	void updateFirmwares(std::vector<uint64_t> ids);
	void updateFirmware(uint64_t id);
	void handleAnnounce(std::shared_ptr<HMWiredPacket> packet);
	bool peerInit(std::shared_ptr<HMWiredPeer> peer);

	virtual PVariable addLink(BaseLib::PRpcClientInfo clientInfo, std::string senderSerialNumber, int32_t senderChannel, std::string receiverSerialNumber, int32_t receiverChannel, std::string name, std::string description);
	virtual PVariable addLink(BaseLib::PRpcClientInfo clientInfo, uint64_t senderID, int32_t senderChannel, uint64_t receiverID, int32_t receiverChannel, std::string name, std::string description);
	virtual PVariable deleteDevice(BaseLib::PRpcClientInfo clientInfo, std::string serialNumber, int32_t flags);
	virtual PVariable deleteDevice(BaseLib::PRpcClientInfo clientInfo, uint64_t peerID, int32_t flags);
	virtual PVariable getDeviceInfo(BaseLib::PRpcClientInfo clientInfo, uint64_t id, std::map<std::string, bool> fields);
	virtual PVariable putParamset(BaseLib::PRpcClientInfo clientInfo, std::string serialNumber, int32_t channel, ParameterGroup::Type::Enum type, std::string remoteSerialNumber, int32_t remoteChannel, PVariable paramset);
	virtual PVariable putParamset(BaseLib::PRpcClientInfo clientInfo, uint64_t peerID, int32_t channel, ParameterGroup::Type::Enum type, uint64_t remoteID, int32_t remoteChannel, PVariable paramset);
	virtual PVariable removeLink(BaseLib::PRpcClientInfo clientInfo, std::string senderSerialNumber, int32_t senderChannel, std::string receiverSerialNumber, int32_t receiverChannel);
	virtual PVariable removeLink(BaseLib::PRpcClientInfo clientInfo, uint64_t senderID, int32_t senderChannel, uint64_t receiverID, int32_t receiverChannel);
	virtual PVariable searchDevices(BaseLib::PRpcClientInfo clientInfo);
	virtual PVariable updateFirmware(BaseLib::PRpcClientInfo clientInfo, std::vector<uint64_t> ids, bool manual);
protected:
	//In table variables
	int32_t _firmwareVersion = 0;
	int32_t _centralAddress = 0;
	std::unordered_map<int32_t, uint8_t> _messageCounter;
	//End

	std::atomic_bool _stopWorkerThread;
	std::thread _workerThread;

	HMWiredPacketManager _receivedPackets;
	HMWiredPacketManager _sentPackets;
	std::atomic_bool _pairing;

	std::mutex _peerInitMutex;

	//Updates:
	std::atomic_bool _updateMode;
	std::mutex _updateFirmwareThreadMutex;
	std::mutex _updateMutex;
	std::thread _updateFirmwareThread;
	//End

	std::mutex _announceThreadMutex;
	std::thread _announceThread;

	virtual void loadPeers();
	virtual void savePeers(bool full);
	virtual void loadVariables();
	virtual void saveVariables();

	std::shared_ptr<HMWiredPeer> createPeer(int32_t address, int32_t firmwareVersion, uint32_t deviceType, std::string serialNumber, bool save = true);
	virtual void worker();
	void deletePeer(uint64_t id);
	virtual void init();
	void lockBus();
	void unlockBus();
};

} /* namespace HMWired */

#endif /* HMWIREDCENTRAL_H_ */
