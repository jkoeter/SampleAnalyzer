#ifndef KAKU_ANALYZER_H
#define KAKU_ANALYZER_H

#include <Analyzer.h>
#include "KaKuAnalyzerSettings.h"
#include "KaKuAnalyzerResults.h"
#include "KaKuSimulationDataGenerator.h"
#include <memory>


class ANALYZER_EXPORT KaKuAnalyzer : public Analyzer2
{
public:
	KaKuAnalyzer();
	virtual ~KaKuAnalyzer();

	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //vars
	KaKuAnalyzerSettings mSettings;
	std::unique_ptr<KaKuAnalyzerResults> mResults;
	AnalyzerChannelData* mSerial;

	KaKuSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;

	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //KAKU_ANALYZER_H
