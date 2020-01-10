// This file contains the code for the JUCE plugin processor.
// 3 Band stereo filtering is implemented in the JUCE callback,
// using a cascading filter design and a convolution utility function.

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ParametricEqAudioProcessor::ParametricEqAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // initialize user interface parameter settings
    addParameter(mLoGainParameter = new AudioParameterFloat("lowgain", "Low Gain", 0, 1.5, 1));
    addParameter(mMidGainParameter = new AudioParameterFloat("midgain", "Mid Gain", 0, 1.5, 1));
    addParameter(mHiGainParameter = new AudioParameterFloat("higain", "High Gain", 0, 1.5, 1));
    
    // initialize state to zero
    for(int i=0; i<MAX_COEF; i++)
    {
        xStateL[i] = 0;
        xStateR[i] = 0;
    }
}

ParametricEqAudioProcessor::~ParametricEqAudioProcessor(){}

//==============================================================================
const String ParametricEqAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ParametricEqAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ParametricEqAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ParametricEqAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ParametricEqAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ParametricEqAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ParametricEqAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ParametricEqAudioProcessor::setCurrentProgram (int index)
{
}

const String ParametricEqAudioProcessor::getProgramName (int index)
{
    return {};
}

void ParametricEqAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void ParametricEqAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ParametricEqAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ParametricEqAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

// Utility function for generic FIR filtering
float ParametricEqAudioProcessor::filter(float* buffer, double* coeffs, int P, int n, int chan)
{
    float sum;
    // filter buffer
    sum = 0;
        for (int i=0; i<=P; i++)
        {
            if((n-i) < 0)
            {
                // Determine whether we're filtering left or right channel
                if(chan == 0)
                {
                    sum += xStateL[MAX_COEF+(n-i)]*coeffs[i];
                }
                else if(chan == 1)
                {
                    sum += xStateR[MAX_COEF+(n-i)]*coeffs[i];
                }
            }
            else
            {
                sum += buffer[n-i]*coeffs[i];
            }
        }
    return sum;
}

void ParametricEqAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // number of samles in buffer
    int numSamp = buffer.getNumSamples();
    
    // variables to keep track of each filter result
    float sumLeftLow = 0;
    float sumRightLow = 0;
    float sumLeftMid = 0;
    float sumRightMid = 0;
    float sumLeftHi = 0;
    float sumRightHi = 0;
    
    // arrays for keeping track of input and output
    float yL1[numSamp];
    float yR1[numSamp];
    float xL[numSamp];
    float xR[numSamp];

    // copy buffer into x
    for(int i=0; i<numSamp; i++)
    {
        xL[i] = buffer.getSample(0, i);
        xR[i] = buffer.getSample(1, i);
    }
    
    // filter signal
    for(int n=0; n<numSamp; n++)
    {
        /***** Begin Left Channel Processing ****/
        // Process low frequencies in left channel
        sumLeftLow = 0;
        // low pass left channel
        sumLeftLow = filter(xL, lowPass, P, n, 0);
        // User scaling of low frequencies in left channel
        sumLeftLow *= mLoGainParameter->get();
        
        // Process mid frequencies in left channel
        sumLeftMid = 0;
        // band pass left channel
        sumLeftMid = filter(xL, bandPass, P, n, 0);
        // User scaling of high frequencies in left channel
        sumLeftMid *= mMidGainParameter->get();
        
        // Process high frequencies in left channel
        sumLeftHi = 0;
        // high pass left channel
        sumLeftHi = filter(xL, hiPass, P2, n, 0);
        //sumLeftHi = filterIIR(xL, hiPassA, hiPassB, A, B, n, 0);
        // User scaling of high frequencies in left channel
        sumLeftHi *= mHiGainParameter->get();
        
        // Sum low mid and high frequencies in left channel
        yL1[n] = sumLeftLow + sumLeftMid + sumLeftHi;
        /**** End Left Channel Processing ****/
        
        
        /**** Begin Right Channel Processing ****/
        // Process low frequencies in right channel
        sumRightLow = 0;
        // low pass right channel
        sumRightLow = filter(xR, lowPass, P, n, 1);
        // User scaling of low frequencies in right channel
        sumRightLow *= mLoGainParameter->get();
        
        // Process mid frequencies in right channel
        sumRightMid = 0;
        // band pass right channel
        sumRightMid = filter(xR, bandPass, P, n, 1);
        // User scaling of low frequencies in right channel
        sumRightMid *= mMidGainParameter->get();
        
        // Process high frequencies in right channel
        sumRightHi = 0;
        // high pass right channel
        sumRightHi = filter(xR, hiPass, P2, n, 1);
        //sumLeftHi = filterIIR(xR, hiPassA, hiPassB, A, B, n, 1);
        // User scaling of high frequencies in right channel
        sumRightHi *= mHiGainParameter->get();
        
        // Sum low mid and high frequencies in right channel
        yR1[n] = sumRightLow + sumRightMid + sumRightHi;
        /**** End Right Channel Processing ****/
    }
    
    // Coppy Y buffer into output buffer
    for(int i=0; i<numSamp; i++)
    {
        buffer.setSample(0, i, yL1[i]);
        buffer.setSample(1, i, yR1[i]);
    }
    
    // update state buffer
    for(int i=0; i<=P; i++)
    {
        xStateL[MAX_COEF-P+i] = xL[numSamp-P+i];
        xStateR[MAX_COEF-P+i] = xR[numSamp-P+i];
    }
}

//==============================================================================
bool ParametricEqAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ParametricEqAudioProcessor::createEditor()
{
    return new ParametricEqAudioProcessorEditor (*this);
}

//==============================================================================
void ParametricEqAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ParametricEqAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ParametricEqAudioProcessor();
}
