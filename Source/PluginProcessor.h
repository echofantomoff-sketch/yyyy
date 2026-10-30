#pragma once
#include <JuceHeader.h>
#include <set>
#include "Presets.h"

// Jednostavan TPT state-variable filter
struct SVF
{
    float s1 = 0.f, s2 = 0.f;
    void reset() { s1 = s2 = 0.f; }
    // mode: 0 = LP, 1 = BP, 2 = HP
    inline float process (float x, float g, float k, int mode)
    {
        const float a1 = 1.0f / (1.0f + g * (g + k));
        const float a2 = g * a1;
        const float a3 = g * a2;
        const float v3 = x - s2;
        const float v1 = a1 * s1 + a2 * v3;
        const float v2 = s2 + a2 * s1 + a3 * v3;
        s1 = 2.0f * v1 - s1;
        s2 = 2.0f * v2 - s2;
        if (mode == 0) return v2;
        if (mode == 1) return v1 * k;
        return x - k * v1 - v2;
    }
};

class AtlasRetroAudioProcessor : public juce::AudioProcessor,
                                 public juce::ChangeBroadcaster
{
public:
    AtlasRetroAudioProcessor();
    ~AtlasRetroAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Atlas Retro"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 4.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // --- preseti ---
    void loadPreset (int bank, int idx);
    int getBank() const { return currentBank; }
    int getPreset() const { return currentPreset; }
    bool isFav (int globalIdx) const { return favs.count (globalIdx) > 0; }
    void toggleFav (int globalIdx);
    void clearFavs() { favs.clear(); }

    juce::AudioProcessorValueTreeState apvts;
    juce::MidiKeyboardState keyboardState;
    std::atomic<float> pitchBend { 0.f }, modWheel { 0.f }, outLevel { 0.f };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    struct Prm
    {
        int osc1Wave = 0, osc2Wave = 0, filtType = 0, polyMode = 0, unison = 1, lfoWave = 0;
        bool sync = false, lfoSync = false;
        float osc1Level = 0.8f, osc2Level = 0.6f, osc2Mul = 1.f;
        float cutoff = 6000.f, resonance = 0.2f, filterEnv = 0.3f;
        float lfoRate = 3.f, lfoDepth = 0.f, detune = 0.f, portamento = 0.f, vibrato = 0.f;
        float chorusDepth = 0.f, chorusRate = 0.8f, delayTime = 0.f, delayFb = 0.3f;
        float reverbSize = 0.5f, reverbMix = 0.2f, pan = 0.f, volume = 0.7f;
    } prm;

    struct Voice
    {
        bool active = false, gate = false;
        int note = -1;
        float freq = 440.f, target = 440.f, vel = 1.f;
        juce::uint64 age = 0;
        float ph1[7] = {}, ph2[7] = {};
        juce::ADSR env;
        SVF fa[2], fb[2];
    };

    static constexpr int maxVoices = 8;
    Voice voices[maxVoices];
    std::vector<int> heldNotes;
    juce::uint64 noteCounter = 0;
    float lastFreq = 0.f;
    double sr = 44100.0;
    float lfoPhase = 0.f, lfoHold = 0.f, vibPhase = 0.f, lfoHz = 3.f;
    float uniMul[7] = {}, uniGL[7] = {}, uniGR[7] = {};
    juce::Random rng;

    juce::dsp::Chorus<float> chorus;
    juce::Reverb reverb;
    std::vector<float> delayBuf[2];
    int delayPos = 0;

    int currentBank = 0, currentPreset = 0;
    std::set<int> favs;

    void updateParams();
    void handleMidi (const juce::MidiMessage& m);
    void noteOn (int note, float vel);
    void noteOff (int note);
    void allNotesOff();
    void renderVoices (float* L, float* R, int start, int n);
    void initVoice (Voice& v, int note, float vel, float f, bool retrigger);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AtlasRetroAudioProcessor)
};
