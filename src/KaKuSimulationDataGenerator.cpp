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
	U32 Tshort = (400 * mSimulationSampleRateHz) / 1000000;  // short pulse is 400uS
	U32 Tlong =  (1100 * mSimulationSampleRateHz) / 1000000;  // long pulse is 1100uS

	//we're currenty high
	//let's move forward a little
	mSerialSimulationData.Advance( Tshort /*samples_per_bit * 10 */ );

	mSerialSimulationData.TransitionIfNeeded( BIT_LOW );  //low-going because we need rising edge for start bit
	mSerialSimulationData.Advance( Tshort ); 

	mSerialSimulationData.TransitionIfNeeded( BIT_LOW );
	mSerialSimulationData.Advance( Tshort );

	for(const KakuBit& bit : mKakuFrame)
	{
	    switch (bit)
		{
		case Low:
			// pulse 1
			mSerialSimulationData.Transition(); // HIGH
			mSerialSimulationData.Advance( Tshort );
			mSerialSimulationData.Transition(); // LOW
			mSerialSimulationData.Advance( Tlong );
			// pulse 2
			mSerialSimulationData.Transition(); // HIGH
			mSerialSimulationData.Advance( Tshort );
			mSerialSimulationData.Transition(); // LOW
			mSerialSimulationData.Advance( Tlong );
			break;
		case High:
			// pulse 1
			mSerialSimulationData.Transition(); // HIGH
			mSerialSimulationData.Advance( Tlong );
			mSerialSimulationData.Transition(); // LOW
			mSerialSimulationData.Advance( Tshort );
			// pulse 2
			mSerialSimulationData.Transition(); // HIGH
			mSerialSimulationData.Advance( Tlong );
			mSerialSimulationData.Transition(); // LOW
			mSerialSimulationData.Advance( Tshort );
			break;
		case X:
			// pulse 1
			mSerialSimulationData.Transition(); // HIGH
			mSerialSimulationData.Advance( Tshort );
			mSerialSimulationData.Transition(); // LOW
			mSerialSimulationData.Advance( Tlong );
			// pulse 2
			mSerialSimulationData.Transition(); // HIGH
			mSerialSimulationData.Advance( Tlong );
			mSerialSimulationData.Transition(); // LOW
			mSerialSimulationData.Advance( Tshort );
			break;
		}
	}

	// Add stop condition
	mSerialSimulationData.Transition();
	mSerialSimulationData.Advance( Tshort );
	mSerialSimulationData.Transition();
	mSerialSimulationData.Advance( 40*Tshort );
	mSerialSimulationData.Transition();
	mSerialSimulationData.Advance( 40*Tshort );
}
