This program implements a stereo 3 band parametric EQ plug-in using the JUCE framework.<br>

Filtering is implemented in a cascading style, with low-pass, band-pass, and high-pass FIR coefficients. <br>

Filter coefficients are defined in PluginProcessing.h with the actual processing implemented in PluginProcessor.cpp <br>

PluginProcessor.cpp contains a convolution helper function that is called in the main callback function: processBlock. <br>
