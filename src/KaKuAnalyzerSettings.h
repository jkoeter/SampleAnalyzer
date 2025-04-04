#ifndef KAKU_ANALYZER_SETTINGS
#define KAKU_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class KaKuAnalyzerSettings : public AnalyzerSettings
{
public:
	KaKuAnalyzerSettings();
	virtual ~KaKuAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel mInputChannel;
	U32 mBitRate;

protected:
	AnalyzerSettingInterfaceChannel	mInputChannelInterface;
	AnalyzerSettingInterfaceInteger	mBitRateInterface;
};

#endif //KAKU_ANALYZER_SETTINGS
