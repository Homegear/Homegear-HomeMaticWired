/* Copyright 2013-2019 Homegear GmbH
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

#include "HMWired.h"
#include "Interfaces.h"
#include "HMWiredCentral.h"
#include "GD.h"

#include <iomanip>

namespace HMWired
{

HMWired::HMWired(BaseLib::SharedObjects* bl, BaseLib::Systems::IFamilyEventSink* eventHandler) : BaseLib::Systems::DeviceFamily(bl, eventHandler, HMWIRED_FAMILY_ID, HMWIRED_FAMILY_NAME)
{
	GD::bl = bl;
	GD::family = this;
	GD::out.init(bl);
	GD::out.setPrefix("Module HomeMatic Wired: ");
	GD::out.printDebug("Debug: Loading module...");
	_physicalInterfaces.reset(new Interfaces(bl, _settings->getPhysicalInterfaceSettings()));
}

HMWired::~HMWired()
{

}

void HMWired::dispose()
{
	if(_disposed) return;
	DeviceFamily::dispose();

	GD::physicalInterface.reset();
}

std::shared_ptr<BaseLib::Systems::ICentral> HMWired::initializeCentral(uint32_t deviceId, int32_t address, std::string serialNumber)
{
	return std::shared_ptr<HMWiredCentral>(new HMWiredCentral(deviceId, serialNumber, address, this));
}

void HMWired::createCentral()
{
	try
	{
		if(_central) return;

		int32_t seedNumber = BaseLib::HelperFunctions::getRandomNumber(1, 9999999);
		std::ostringstream stringstream;
		stringstream << "VWC" << std::setw(7) << std::setfill('0') << std::dec << seedNumber;
		std::string serialNumber(stringstream.str());

		_central.reset(new HMWiredCentral(0, serialNumber, 1, this));
		GD::out.printMessage("Created HomeMatic Wired central with id " + std::to_string(_central->getId()) + ", address 0x" + BaseLib::HelperFunctions::getHexString(1, 8) + " and serial number " + serialNumber);
	}
	catch(const std::exception& ex)
    {
    	GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
    }
    catch(...)
    {
    	GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
    }
}

PVariable HMWired::getPairingInfo()
{
	try
	{
		if(!_central) return std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		PVariable info = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);

		//{{{ General
		info->structValue->emplace("searchInterfaces", std::make_shared<BaseLib::Variable>(false));
		//}}}

		//{{{ Pairing methods
		PVariable pairingMethods = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		pairingMethods->structValue->emplace("searchDevices", std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct));
        info->structValue->emplace("pairingMethods", pairingMethods);
		//}}}

		//{{{ interfaces
		auto interfaces = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);

		//{{{ HMW-LGW
		auto interface = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		interface->structValue->emplace("name", std::make_shared<BaseLib::Variable>(std::string("HMW-LGW")));
		interface->structValue->emplace("ipDevice", std::make_shared<BaseLib::Variable>(true));

		auto field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("pos", std::make_shared<BaseLib::Variable>(0));
		field->structValue->emplace("label", std::make_shared<BaseLib::Variable>(std::string("l10n.homematicwired.pairingInfo.id")));
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("string")));
		interface->structValue->emplace("id", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("pos", std::make_shared<BaseLib::Variable>(1));
		field->structValue->emplace("label", std::make_shared<BaseLib::Variable>(std::string("l10n.homematicwired.pairingInfo.hostname")));
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("string")));
		interface->structValue->emplace("host", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("pos", std::make_shared<BaseLib::Variable>(2));
		field->structValue->emplace("label", std::make_shared<BaseLib::Variable>(std::string("l10n.homematicwired.pairingInfo.key")));
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("string")));
		interface->structValue->emplace("lanKey", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("string")));
		field->structValue->emplace("const", std::make_shared<BaseLib::Variable>(std::string("1000")));
		interface->structValue->emplace("port", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("integer")));
		field->structValue->emplace("const", std::make_shared<BaseLib::Variable>(8));
		interface->structValue->emplace("responseDelay", field);

		interfaces->structValue->emplace("hmwlgw", interface);
		//}}}

		//{{{ USB module
		interface = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		interface->structValue->emplace("name", std::make_shared<BaseLib::Variable>(std::string("RS485 USB")));
		interface->structValue->emplace("ipDevice", std::make_shared<BaseLib::Variable>(false));

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("pos", std::make_shared<BaseLib::Variable>(0));
		field->structValue->emplace("label", std::make_shared<BaseLib::Variable>(std::string("l10n.homematicwired.pairingInfo.id")));
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("string")));
		interface->structValue->emplace("id", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("pos", std::make_shared<BaseLib::Variable>(1));
		field->structValue->emplace("label", std::make_shared<BaseLib::Variable>(std::string("l10n.homematicwired.pairingInfo.device")));
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("string")));
		interface->structValue->emplace("device", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("boolean")));
		field->structValue->emplace("const", std::make_shared<BaseLib::Variable>(true));
		interface->structValue->emplace("oneWay", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("integer")));
		field->structValue->emplace("const", std::make_shared<BaseLib::Variable>(20));
		interface->structValue->emplace("responseDelay", field);

		interfaces->structValue->emplace("rs485usb", interface);
		//}}}

		//{{{ Serial module
		interface = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		interface->structValue->emplace("name", std::make_shared<BaseLib::Variable>(std::string("RS485 UART")));
		interface->structValue->emplace("ipDevice", std::make_shared<BaseLib::Variable>(false));

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("pos", std::make_shared<BaseLib::Variable>(0));
		field->structValue->emplace("label", std::make_shared<BaseLib::Variable>(std::string("l10n.homematicwired.pairingInfo.id")));
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("string")));
		interface->structValue->emplace("id", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("pos", std::make_shared<BaseLib::Variable>(1));
		field->structValue->emplace("label", std::make_shared<BaseLib::Variable>(std::string("l10n.homematicwired.pairingInfo.device")));
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("string")));
		interface->structValue->emplace("device", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("pos", std::make_shared<BaseLib::Variable>(2));
		field->structValue->emplace("label", std::make_shared<BaseLib::Variable>(std::string("l10n.homematicwired.pairingInfo.gpio1")));
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("integer")));
		field->structValue->emplace("default", std::make_shared<BaseLib::Variable>(17));
		interface->structValue->emplace("gpio1", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("pos", std::make_shared<BaseLib::Variable>(3));
		field->structValue->emplace("label", std::make_shared<BaseLib::Variable>(std::string("l10n.homematicwired.pairingInfo.enablerxvalue")));
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("boolean")));
		field->structValue->emplace("default", std::make_shared<BaseLib::Variable>(false));
		interface->structValue->emplace("enableRxValue", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("pos", std::make_shared<BaseLib::Variable>(4));
		field->structValue->emplace("label", std::make_shared<BaseLib::Variable>(std::string("l10n.homematicwired.pairingInfo.gpio2")));
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("integer")));
		field->structValue->emplace("default", std::make_shared<BaseLib::Variable>(18));
		interface->structValue->emplace("gpio2", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("pos", std::make_shared<BaseLib::Variable>(5));
		field->structValue->emplace("label", std::make_shared<BaseLib::Variable>(std::string("l10n.homematicwired.pairingInfo.enabletxvalue")));
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("boolean")));
		field->structValue->emplace("default", std::make_shared<BaseLib::Variable>(true));
		interface->structValue->emplace("enableTxValue", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("boolean")));
		field->structValue->emplace("const", std::make_shared<BaseLib::Variable>(false));
		interface->structValue->emplace("oneWay", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("boolean")));
		field->structValue->emplace("const", std::make_shared<BaseLib::Variable>(true));
		interface->structValue->emplace("fastSending", field);

		field = std::make_shared<BaseLib::Variable>(BaseLib::VariableType::tStruct);
		field->structValue->emplace("type", std::make_shared<BaseLib::Variable>(std::string("integer")));
		field->structValue->emplace("const", std::make_shared<BaseLib::Variable>(13));
		interface->structValue->emplace("responseDelay", field);

		interfaces->structValue->emplace("rs485", interface);
		//}}}

		info->structValue->emplace("interfaces", interfaces);
		//}}}

		return info;
	}
	catch(const std::exception& ex)
	{
		GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch(...)
	{
		GD::out.printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
	return Variable::createError(-32500, "Unknown application error.");
}
}
