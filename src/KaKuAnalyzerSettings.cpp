#include "KaKuAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


KaKuAnalyzerSettings::KaKuAnalyzerSettings()
:	mInputChannel( UNDEFINED_CHANNEL ),
	mInputChannelInterface()
{
	mInputChannelInterface.SetTitleAndTooltip( "Classic KaKu", "Standard Classic KaKu" );
	mInputChannelInterface.SetChannel( mInputChannel );

	AddInterface( &mInputChannelInterface );

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( mInputChannel, "Classic KaKu", false );
}

KaKuAnalyzerSettings::~KaKuAnalyzerSettings()
{
}

bool KaKuAnalyzerSettings::SetSettingsFromInterfaces()
{
	mInputChannel = mInputChannelInterface.GetChannel();

	ClearChannels();
	AddChannel( mInputChannel, "Classic KaKu", true );

	return true;
}

void KaKuAnalyzerSettings::UpdateInterfacesFromSettings()
{
	mInputChannelInterface.SetChannel( mInputChannel );
}

void KaKuAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	text_archive >> mInputChannel;

	ClearChannels();
	AddChannel( mInputChannel, "Classic KaKu", true );

	UpdateInterfacesFromSettings();
}

const char* KaKuAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << mInputChannel;

	return SetReturnString( text_archive.GetString() );
}
