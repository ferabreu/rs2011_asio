#pragma once

#include "RSBaseDeviceEnum.h"
#include "RSAsioDevice.h"

#include <optional>

struct RSAsioOutputConfig
{
	std::string asioDriverName;
	unsigned baseChannel = 0;
	std::optional<unsigned> altBaseChannel;
	unsigned numChannels = 2;
	bool enableSoftwareEndpointVolumeControl = true;
	bool enableSoftwareMasterVolumeControl = true;
	int softwareMasterVolumePercent = 100;
	std::optional<bool> enableRefCountHack;
};

struct RSAsioInputConfig
{
	std::string asioDriverName;
	unsigned useChannel = 0;
	bool enableSoftwareEndpointVolumeControl = true;
	bool enableSoftwareMasterVolumeControl = true;
	int softwareMasterVolumePercent = 100;
	bool microphone = false;
	std::optional<bool> enableRefCountHack;
	std::string wasapiRedirectId; // if non-empty: when wasapiId is also set, used as the friendly name reported to the game;
	                              // when wasapiId is empty, used as a substring redirect pattern against real WASAPI device IDs/names
	std::string wasapiId;         // if non-empty, use as device endpoint ID (e.g. {0.0.1.00000000}.{GUID}) for cable-free RS2011
};

// Registry of WASAPI device ID → IMMDevice (ASIO-backed) for audio client redirect.
// Populated by RSAsioDeviceEnum when WasapiDevice= is configured in RS_ASIO.ini.
void RegisterWasapiRedirect(const std::wstring& wasapiDeviceSubId, IMMDevice* asioDevice);
void ClearWasapiRedirects();
// Checks deviceId and (optionally) deviceFriendlyName against registered redirect patterns.
// The WasapiDevice= INI value is matched as a case-insensitive substring against both the
// full device ID string and the device friendly name, so users may specify either a GUID
// fragment or a human-readable name (e.g. "Rocksmith USB Guitar Adapter").
IMMDevice* GetWasapiRedirectDevice(const std::wstring& deviceId, const std::wstring& deviceFriendlyName = std::wstring());

struct RSAsioConfig
{
	RSAsioOutputConfig output;
	std::array<RSAsioInputConfig, 3> inputs;
	RSAsioDevice::BufferSizeMode bufferMode = RSAsioDevice::BufferSizeMode_Driver;
	unsigned customBufferSize = 128;
};

struct RSConfig
{
	std::optional<bool> enableWasapiOutputs = false;
	bool enableWasapiInputs = false;
	bool enableAsioOutput = false;
	bool enableAsioInputs = false;
	RSAsioConfig asioConfig;
};

class RSAsioDeviceEnum : public RSBaseDeviceEnum
{
public:
	void SetConfig(const RSAsioConfig& config);

protected:
	virtual void UpdateAvailableDevices() override;

	RSAsioConfig m_Config;
};
