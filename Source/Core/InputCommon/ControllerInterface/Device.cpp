
#include "Device.h"

#include <string>
#include <sstream>

namespace ciface
{
namespace Core
{

//
// Device :: ~Device
//
// Destructor, delete all inputs/outputs on device destruction
//
Device::~Device()
{
	// delete inputs
	for (Device::Input* input : m_inputs)
		delete input;

	// delete outputs
	for (Device::Output* output: m_outputs)
		delete output;
}

void Device::AddInput(Device::Input* const i)
{
	m_inputs.push_back(i);
}

void Device::AddOutput(Device::Output* const o)
{
	m_outputs.push_back(o);
}

Device::Input* Device::FindInput(const std::string &name) const
{
	for (Input* input : m_inputs)
	{
		if (input->GetName() == name)
			return input;
	}

	return NULL;
}

Device::Output* Device::FindOutput(const std::string &name) const
{
	for (Output* output : m_outputs)
	{
		if (output->GetName() == name)
			return output;
	}

	return NULL;
}

//
// Device :: ClearInputState
//
// Device classes should override this function
// ControllerInterface will call this when the device returns failure during UpdateInput
// used to try to set all buttons and axes to their default state when user unplugs a gamepad during play
// buttons/axes that were held down at the time of unplugging should be seen as not pressed after unplugging
//
void Device::ClearInputState()
{
	// this is going to be called for every UpdateInput call that fails
	// kinda slow but, w/e, should only happen when user unplugs a device while playing
}

//
// DeviceQualifier :: ToString
//
// Get string from a device qualifier / serialize
//
std::string DeviceQualifier::ToString() const
{
	if (source.empty() && (cid < 0) && name.empty())
		return "";

	std::ostringstream ss;
	ss << source << '/';
	if (cid > -1)
		ss << cid;
	ss << '/' << name;

	return ss.str();
}

//
// DeviceQualifier :: FromString
//
// Set a device qualifier from a string / unserialize
//
void DeviceQualifier::FromString(const std::string& str)
{
	std::istringstream ss(str);

	std::getline(ss, source = "", '/');

	// silly
	std::getline(ss, name, '/');
	std::istringstream(name) >> (cid = -1);

	std::getline(ss, name = "");
}

//
// DeviceQualifier :: FromDevice
//
// Set a device qualifier from a device
//
void DeviceQualifier::FromDevice(const Device* const dev)
{
	name = dev->GetName();
	cid = dev->GetId();
	source= dev->GetSource();
}

bool DeviceQualifier::operator==(const Device* const dev) const
{
	if (dev->GetId() == cid)
		if (dev->GetName() == name)
			if (dev->GetSource() == source)
				return true;

	return false;
}

bool DeviceQualifier::operator==(const DeviceQualifier& devq) const
{
	if (cid == devq.cid)
		if (name == devq.name)
			if (source == devq.source)
				return true;

	return false;
}

Device* DeviceContainer::FindDevice(const DeviceQualifier& devq) const
{
	for (Device* d : m_devices)
	{
		if (devq == d)
			return d;
	}

	return NULL;
}

Device::Input* DeviceContainer::FindInput(const std::string& name, const Device* def_dev) const
{
	if (def_dev)
	{
		Device::Input* const inp = def_dev->FindInput(name);
		if (inp)
			return inp;
	}

	for (Device* d : m_devices)
	{
		Device::Input* const i = d->FindInput(name);

		if (i)
			return i;
	}

	return NULL;
}

Device::Output* DeviceContainer::FindOutput(const std::string& name, const Device* def_dev) const
{
	return def_dev->FindOutput(name);
}

}
}
