#include "KaKuAnalyzer.h"
#include "KaKuAnalyzerSettings.h"
#include <AnalyzerChannelData.h>

KaKuAnalyzer::KaKuAnalyzer()
:	Analyzer2(),  
	mSettings(),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( &mSettings );
}

KaKuAnalyzer::~KaKuAnalyzer()
{
	KillThread();
}

void KaKuAnalyzer::SetupResults()
{
	// SetupResults is called each time the analyzer is run. Because the same instance can be used for multiple runs, we need to clear the results each time.
	mResults.reset(new KaKuAnalyzerResults( this, &mSettings ));
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings.mInputChannel );
}

void KaKuAnalyzer::WorkerThread()
{
	mSampleRateHz = GetSampleRate();
	mSerial = GetAnalyzerChannelData( mSettings.mInputChannel );

	for( ; ; )
	{
		U64 MinSamplesPerBit = ( 200 * mSampleRateHz) / 1000000;
		U64 MaxSamplesPerBit = (1200 * mSampleRateHz) / 1000000;
		U64 MaxSamplesShortPulse = (600 * mSampleRateHz) / 1000000;
		
		if( mSerial->GetBitState() == BIT_HIGH ) // we need a rising edge first so we need to be low
			mSerial->AdvanceToNextEdge();

		mSerial->AdvanceToNextEdge(); // rising edge
		U64 pulse1_starting_sample = mSerial->GetSampleNumber();

		//mSerial->AdvanceToNextEdge(); // falling edge
		U64 pulse1_ending_sample = mSerial->GetSampleOfNextEdge();

		U64 pulse1_duration = pulse1_ending_sample - pulse1_starting_sample;
		// check for pulse validity
		if ((pulse1_duration < MinSamplesPerBit) || (pulse1_duration > MaxSamplesPerBit))
			continue;
		mSerial->AdvanceToNextEdge();

		U64 pulse2_ending_sample = mSerial->GetSampleOfNextEdge();
		U64 pulse2_duration = pulse2_ending_sample - pulse1_ending_sample;
		// check for pulse validity
		if ((pulse2_duration < MinSamplesPerBit) || (pulse2_duration > MaxSamplesPerBit))
			continue;
		mSerial->AdvanceToNextEdge(); // rising edge

		U64 pulse3_ending_sample = mSerial->GetSampleOfNextEdge();

		U64 pulse3_duration = pulse3_ending_sample - pulse2_ending_sample;
		// check for pulse validity
		if ((pulse3_duration < MinSamplesPerBit) || (pulse3_duration > MaxSamplesPerBit))
			continue;
		mSerial->AdvanceToNextEdge(); // rising edge
	
		U64 pulse4_ending_sample = mSerial->GetSampleOfNextEdge(); // rising edge --> dont move, we need this edge the next loop

		U64 pulse4_duration = pulse4_ending_sample - pulse3_ending_sample;
		// check for pulse validity
		if ((pulse4_duration < MinSamplesPerBit) || (pulse4_duration > MaxSamplesPerBit))
			continue;

		/* Determine bit-value */
		U8 value = 9;
		if ((pulse1_duration < pulse2_duration) &&
		    (pulse3_duration < pulse4_duration))
			value = 0;
		if ((pulse1_duration > pulse2_duration) &&
		    (pulse3_duration > pulse4_duration))
			value = 1;

		if ((pulse1_duration < pulse2_duration) &&
		    (pulse3_duration > pulse4_duration))
			value = 2;


		//we have a bit to save. 
		Frame frame;
		frame.mData1 = value;
		frame.mFlags = 0;
		frame.mStartingSampleInclusive = pulse1_starting_sample;
		frame.mEndingSampleInclusive = pulse4_ending_sample;

		mResults->AddFrame( frame );
		mResults->CommitResults();
		ReportProgress( frame.mEndingSampleInclusive );
	}
}

bool KaKuAnalyzer::NeedsRerun()
{
	return false;
}

U32 KaKuAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), &mSettings );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
}

U32 KaKuAnalyzer::GetMinimumSampleRateHz()
{
	return 100000; // 100KHz
}

const char* KaKuAnalyzer::GetAnalyzerName() const
{
	return "Classic KaKu";
}

const char* GetAnalyzerName()
{
	return "Classic KaKu";
}

Analyzer* CreateAnalyzer()
{
	return new KaKuAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}