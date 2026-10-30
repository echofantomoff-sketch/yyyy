#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

// UI je nacrtan u koordinatama originalnog dizajna (1536 x 1024)
// i skalira se na velicinu prozora.
class AtlasRetroAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit AtlasRetroAudioProcessorEditor (AtlasRetroAudioProcessor&);
    ~AtlasRetroAudioProcessorEditor() override;

    void resized() override;

private:
    struct Content;
    AtlasRetroAudioProcessor& proc;
    std::unique_ptr<Content> content;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AtlasRetroAudioProcessorEditor)
};
