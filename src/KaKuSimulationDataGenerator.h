#ifndef KAKU_SIMULATION_DATA_GENERATOR
#define KAKU_SIMULATION_DATA_GENERATOR

#include <SimulationChannelDescriptor.h>
#include <string>
class KaKuAnalyzerSettings;

class KaKuSimulationDataGenerator
{
public:
	KaKuSimulationDataGenerator();
	~KaKuSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, KaKuAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channel );

protected:
	KaKuAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;

protected:
	void CreateSerialByte();
	std::string mSerialText;
	U32 mStringIndex;

	SimulationChannelDescriptor mSerialSimulationData;

};
#endif //KAKU_SIMULATION_DATA_GENERATOR