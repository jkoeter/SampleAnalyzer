#include "KaKuSimulationDataGenerator.h"
#include "KaKuAnalyzerSettings.h"

#include <AnalyzerHelpers.h>

KaKuSimulationDataGenerator::KaKuSimulationDataGenerator()
:	mSerialText( "My first analyzer, woo hoo!" ),
	mStringIndex( 0 ),
	mKakuFrame({Low, Low, Low, Low, High, High, Low, Low, X, X, X})
{
}

KaKuSimulationDataGenerator::~KaKuSimulationDataGenerator()
{
}

void KaKuSimulationDataGenerator::Initialize( U32 simulation_sample_rate, KaKuAnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	mSerialSimulationData.SetChannel( mSettings->mInputChannel );
	mSerialSimulationData.SetSampleRate( simulation_sample_rate );
	mSerialSimulationData.SetInitialBitState( BIT_HIGH );
}

U32 KaKuSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	while( mSerialSimulationData.GetCurrentSampleNumber() < adjusted_largest_sample_requested )
	{
		CreateKakuFrame();
	}

	*simulation_channel = &mSerialSimulationData;
	return 1;
}

void KaKuSimulationDataGenerator::CreateKakuFrame()
{
	U32 T = (200 * mSimulationSampleRateHz) / 1000000;  // short pulse is 200uS

	//we're currenty high
	//let's move forward a little
	mSerialSimulationData.Advance( T /*samples_per_bit * 10 */ );

	mSerialSimulationData.TransitionIfNeeded( BIT_LOW );  //low-going because we need rising edge for start bit
	mSerialSimulationData.Advance( T ); 

	mSerialSimulationData.TransitionIfNeeded( BIT_LOW );
	mSerialSimulationData.Advance( T );

	for(const KakuBit& bit : mKakuFrame)
	{
	    switch (bit)
		{
		case Low:
			// pulse 1
			mSerialSimulationData.Transition(); // HIGH
			mSerialSimulationData.Advance( 1*T );
			mSerialSimulationData.Transition(); // LOW
			mSerialSimulationData.Advance( 3*T );
			// pulse 2
			mSerialSimulationData.Transition(); // HIGH
			mSerialSimulationData.Advance( 1*T );
			mSerialSimulationData.Transition(); // LOW
			mSerialSimulationData.Advance( 3*T );
			break;
		case High:
			// pulse 1
			mSerialSimulationData.Transition(); // HIGH
			mSerialSimulationData.Advance( 3*T );
			mSerialSimulationData.Transition(); // LOW
			mSerialSimulationData.Advance( 1*T );
			// pulse 2
			mSerialSimulationData.Transition(); // HIGH
			mSerialSimulationData.Advance( 3*T );
			mSerialSimulationData.Transition(); // LOW
			mSerialSimulationData.Advance( 1*T );
			break;
		case X:
			// pulse 1
			mSerialSimulationData.Transition(); // HIGH
			mSerialSimulationData.Advance( 1*T );
			mSerialSimulationData.Transition(); // LOW
			mSerialSimulationData.Advance( 3*T );
			// pulse 2
			mSerialSimulationData.Transition(); // HIGH
			mSerialSimulationData.Advance( 3*T );
			mSerialSimulationData.Transition(); // LOW
			mSerialSimulationData.Advance( 1*T );
			break;
		}
	}

	// Add stop condition
	mSerialSimulationData.Transition();
	mSerialSimulationData.Advance( T );
	mSerialSimulationData.Transition();
	mSerialSimulationData.Advance( 40*T );
	mSerialSimulationData.Transition();
	mSerialSimulationData.Advance( 40*T );
}
