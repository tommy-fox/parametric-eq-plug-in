// This is the header definition for the audio processing functions
// and class members, including the filter coefficients for each of the
// 3 EQ bands: low pass, band pass, and high pass

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

// Maximum number of coefficients allowed in FIR filter
#define MAX_COEF 41

//==============================================================================
/**
*/
class ParametricEqAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    ParametricEqAudioProcessor();
    ~ParametricEqAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    float filter(float* buffer, double* coeffs, int P, int n, int chan);
    
    float filterIIR(float* buffer, double* coeffsA, double* coeffsB, int A, int B, int n, int chan);

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParametricEqAudioProcessor)
    
    // Parameters for user control of high, mid, and low gain
    AudioParameterFloat* mLoGainParameter;
    AudioParameterFloat* mMidGainParameter;
    AudioParameterFloat* mHiGainParameter;
    
    // Necessary state information for filter function
    double xStateL[MAX_COEF];
    double xStateR[MAX_COEF];
    
    // P corresponds to low and band pass Coefficients
    // P2 corresonds to high pass coefficients
    const static int P = 41;
    const static int P2 = 33;
    
    double lowPass[MAX_COEF] =
    {
        -0.0003919,
        -0.0007032,
        -0.001203
        -0.001776,
        -0.002304,
        -0.002603,
        -0.002426,
        -0.001483,
        0.0005265,
        0.003875,
        0.008755,
        0.01524,
        0.02323,
        0.03247,
        0.04251,
        0.05276,
        0.06252,
        0.07107,
        0.07774,
        0.08198,
        0.08344,
        0.08198,
        0.07774,
        0.07107,
        0.06252,
        0.05276,
        0.04251,
        0.03247,
        0.02323,
        0.01524,
        0.008755,
        0.003875,
        0.0005265,
        -0.001483,
        -0.002426,
        -0.002603,
        -0.002304,
        -0.001776,
        -0.001203,
        -0.0007032,
        -0.0003919
    };
    
    double bandPass[MAX_COEF] =
    {
        -4.652e-07,
        -5.335e-06,
        -3.298e-05,
        -0.0001441,
        -0.0004942,
        -0.001407,
        -0.003432,
        -0.007326,
        -0.01387,
        -0.02345,
        -0.03554,
        -0.04819,
        -0.05791,
        -0.06022,
        -0.05095,
        -0.02796,
        0.007339,
        0.04948,
        0.08984,
        0.119,
        0.1296,
        0.119,
        0.08984,
        0.04948,
        0.007339,
        -0.02796,
        -0.05095,
        -0.06022,
        -0.05791,
        -0.04819,
        -0.03554,
        -0.02345,
        -0.01387,
        -0.007326,
        -0.003432,
        -0.001407,
        -0.0004942,
        -0.0001441,
        -3.298e-05,
        -5.335e-06,
        -4.652e-07
    };
    
    double hiPass[MAX_COEF] =
    {
        0.00363,
        0.001044,
        -0.001626,
        -0.005666,
        -0.008914,
        -0.00845,
        -0.002127,
        0.009719,
        0.02319,
        0.03131,
        0.02627,
        0.00259,
        -0.03974,
        -0.09402,
        -0.1481,
        -0.188,
        0.7973,
        -0.188,
        -0.1481,
        -0.09402,
        -0.03974,
        0.00259,
        0.02627,
        0.03131,
        0.02319,
        0.009719,
        -0.002127,
        -0.00845,
        -0.008914,
        -0.005666,
        -0.001626,
        0.001044,
        0.00363
    };
};
