#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

namespace
{
inline float polyBlep (float t, float dt)
{
    if (t < dt)        { t /= dt; return t + t - t * t - 1.0f; }
    if (t > 1.0f - dt) { t = (t - 1.0f) / dt; return t * t + t + t + 1.0f; }
    return 0.0f;
}

// 0 = saw, 1 = square, 2 = triangle, 3 = sine
inline float oscSample (int w, float ph, float dt)
{
    switch (w)
    {
        case 0: return (2.0f * ph - 1.0f) - polyBlep (ph, dt);
        case 1:
        {
            float v = ph < 0.5f ? 1.0f : -1.0f;
            v += polyBlep (ph, dt);
            float p2 = ph + 0.5f; if (p2 >= 1.0f) p2 -= 1.0f;
            v -= polyBlep (p2, dt);
            return v * 0.8f;
        }
        case 2: return 4.0f * std::abs (ph - 0.5f) - 1.0f;
        default: return std::sin (ph * MathConstants<float>::twoPi);
    }
}

inline float runFilter (SVF* f, int type, float x, float g, float k1, float k2)
{
    switch (type)
    {
        case 0:  return f[1].process (f[0].process (x, g, k1, 0), g, k2, 0); // LP 24
        case 1:  return f[0].process (x, g, k1, 0);                          // LP 12
        case 2:  return f[0].process (x, g, k1, 2);                          // HP 12
        default: return f[0].process (x, g, k1, 1);                          // BP 12
    }
}
} // namespace

//==============================================================================
AudioProcessorValueTreeState::ParameterLayout AtlasRetroAudioProcessor::createLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> p;
    auto pid = [] (const char* s) { return ParameterID { s, 1 }; };

    auto fl = [&] (const char* id, const char* name, float lo, float hi, float def, float skew = 1.0f)
    {
        p.push_back (std::make_unique<AudioParameterFloat> (pid (id), name, NormalisableRange<float> (lo, hi, 0.0f, skew), def));
    };
    auto ch = [&] (const char* id, const char* name, StringArray items, int def)
    {
        p.push_back (std::make_unique<AudioParameterChoice> (pid (id), name, items, def));
    };
    auto bo = [&] (const char* id, const char* name, bool def)
    {
        p.push_back (std::make_unique<AudioParameterBool> (pid (id), name, def));
    };

    ch ("osc1Wave", "Osc1 Wave", { "Saw", "Square", "Triangle", "Sine" }, 0);
    fl ("osc1Level", "Osc1 Level", 0.f, 1.f, 0.8f);
    ch ("osc2Wave", "Osc2 Wave", { "Saw", "Square", "Triangle", "Sine" }, 0);
    fl ("osc2Tune", "Osc2 Tune", -24.f, 24.f, 0.08f);
    fl ("osc2Level", "Osc2 Level", 0.f, 1.f, 0.6f);
    bo ("sync", "Osc Sync", false);

    fl ("cutoff", "Cutoff", 20.f, 20000.f, 6000.f, 0.3f);
    fl ("resonance", "Resonance", 0.f, 1.f, 0.2f);
    ch ("filterType", "Filter Type", { "LP 24dB", "LP 12dB", "HP 12dB", "BP 12dB" }, 0);
    fl ("filterEnv", "Filter Env", 0.f, 1.f, 0.3f);

    fl ("attack", "Attack", 0.001f, 5.f, 0.005f, 0.35f);
    fl ("decay", "Decay", 0.001f, 5.f, 0.4f, 0.35f);
    fl ("sustain", "Sustain", 0.f, 1.f, 0.8f);
    fl ("release", "Release", 0.005f, 8.f, 0.25f, 0.35f);

    fl ("lfoRate", "LFO Rate", 0.05f, 20.f, 3.f, 0.4f);
    fl ("lfoDepth", "LFO Depth", 0.f, 1.f, 0.f);
    ch ("lfoWave", "LFO Waveform", { "Sine", "Triangle", "Saw", "Square", "S&H" }, 0);
    bo ("lfoSync", "LFO Sync", false);

    fl ("pan", "Pan", -1.f, 1.f, 0.f);
    fl ("volume", "Volume", 0.f, 1.f, 0.7f);

    ch ("polyphony", "Polyphony", { "Poly", "Mono", "Legato" }, 0);
    ch ("unison", "Unison", { "1", "2", "3", "4", "5", "6", "7" }, 0);
    fl ("detune", "Detune", 0.f, 1.f, 0.2f);
    fl ("portamento", "Portamento", 0.f, 2.f, 0.f, 0.4f);
    fl ("vibrato", "Vibrato", 0.f, 1.f, 0.5f);

    fl ("chorusDepth", "Chorus Depth", 0.f, 1.f, 0.3f);
    fl ("chorusRate", "Chorus Rate", 0.1f, 5.f, 0.8f, 0.5f);
    fl ("delayTime", "Delay Time", 0.f, 1.f, 0.f);
    fl ("delayFeedback", "Delay Feedback", 0.f, 0.9f, 0.3f);
    fl ("reverbSize", "Reverb Size", 0.f, 1.f, 0.5f);
    fl ("reverbMix", "Reverb Mix", 0.f, 1.f, 0.2f);

    return { p.begin(), p.end() };
}

//==============================================================================
AtlasRetroAudioProcessor::AtlasRetroAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "ATLAS_RETRO", createLayout())
{
    loadPreset (0, 0);
}

bool AtlasRetroAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == AudioChannelSet::stereo();
}

void AtlasRetroAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    for (auto& v : voices)
    {
        v.env.setSampleRate (sr);
        v.env.reset();
        v.active = false;
        for (auto& f : v.fa) f.reset();
        for (auto& f : v.fb) f.reset();
    }
    heldNotes.clear();

    dsp::ProcessSpec spec { sr, (uint32) samplesPerBlock, 2 };
    chorus.prepare (spec);
    chorus.reset();
    reverb.setSampleRate (sr);
    reverb.reset();

    const int size = (int) (sr * 1.0) + 4;
    for (auto& b : delayBuf) b.assign ((size_t) size, 0.0f);
    delayPos = 0;
}

//==============================================================================
void AtlasRetroAudioProcessor::updateParams()
{
    auto gv = [this] (const char* id) { return apvts.getRawParameterValue (id)->load(); };

    prm.osc1Wave   = (int) gv ("osc1Wave");
    prm.osc2Wave   = (int) gv ("osc2Wave");
    prm.osc1Level  = gv ("osc1Level");
    prm.osc2Level  = gv ("osc2Level");
    prm.osc2Mul    = std::exp2 (gv ("osc2Tune") / 12.0f);
    prm.sync       = gv ("sync") > 0.5f;
    prm.cutoff     = gv ("cutoff");
    prm.resonance  = gv ("resonance");
    prm.filtType   = (int) gv ("filterType");
    prm.filterEnv  = gv ("filterEnv");
    prm.lfoRate    = gv ("lfoRate");
    prm.lfoDepth   = gv ("lfoDepth");
    prm.lfoWave    = (int) gv ("lfoWave");
    prm.lfoSync    = gv ("lfoSync") > 0.5f;
    prm.pan        = gv ("pan");
    prm.volume     = gv ("volume");
    prm.polyMode   = (int) gv ("polyphony");
    prm.unison     = jlimit (1, 7, (int) gv ("unison") + 1);
    prm.detune     = gv ("detune");
    prm.portamento = gv ("portamento");
    prm.vibrato    = gv ("vibrato");
    prm.chorusDepth= gv ("chorusDepth");
    prm.chorusRate = gv ("chorusRate");
    prm.delayTime  = gv ("delayTime");
    prm.delayFb    = gv ("delayFeedback");
    prm.reverbSize = gv ("reverbSize");
    prm.reverbMix  = gv ("reverbMix");

    ADSR::Parameters ap { gv ("attack"), gv ("decay"), gv ("sustain"), gv ("release") };
    for (auto& v : voices) v.env.setParameters (ap);

    const int nU = prm.unison;
    for (int u = 0; u < nU; ++u)
    {
        const float pos = nU > 1 ? ((float) u / (float) (nU - 1)) * 2.0f - 1.0f : 0.0f;
        uniMul[u] = std::exp2 (pos * prm.detune * 40.0f / 1200.0f);
        const float angle = (pos * 0.8f + 1.0f) * MathConstants<float>::pi * 0.25f;
        uniGL[u] = std::cos (angle) * 1.4142f;
        uniGR[u] = std::sin (angle) * 1.4142f;
    }

    if (prm.lfoSync)
    {
        double bpm = 120.0;
        if (auto* ph = getPlayHead())
            if (auto pos = ph->getPosition())
                if (auto b = pos->getBpm())
                    bpm = *b;
        const float q = std::pow (2.0f, std::round (std::log2 (std::max (prm.lfoRate, 0.05f))));
        lfoHz = (float) (bpm / 60.0) * q / 4.0f;
    }
    else
        lfoHz = prm.lfoRate;
}

//==============================================================================
void AtlasRetroAudioProcessor::initVoice (Voice& v, int note, float vel, float f, bool retrigger)
{
    const bool wasActive = v.active;
    v.note = note;
    v.target = f;
    v.vel = vel;
    v.age = ++noteCounter;
    if (! wasActive)
    {
        v.freq = (prm.portamento > 0.001f && lastFreq > 0.0f) ? lastFreq : f;
        for (int u = 0; u < 7; ++u)
        {
            v.ph1[u] = u == 0 ? 0.0f : rng.nextFloat();
            v.ph2[u] = u == 0 ? 0.0f : rng.nextFloat();
        }
        for (auto& x : v.fa) x.reset();
        for (auto& x : v.fb) x.reset();
    }
    v.active = true;
    v.gate = true;
    if (retrigger)
    {
        v.env.reset();
        v.env.noteOn();
    }
}

void AtlasRetroAudioProcessor::noteOn (int note, float vel)
{
    const float f = 440.0f * std::exp2 (((float) note - 69.0f) / 12.0f);

    if (prm.polyMode == 0)
    {
        Voice* v = nullptr;
        for (auto& x : voices) if (! x.active) { v = &x; break; }
        if (v == nullptr)
        {
            v = &voices[0];
            for (auto& x : voices) if (x.age < v->age) v = &x;
            v->active = false; // steal
        }
        initVoice (*v, note, vel, f, true);
    }
    else
    {
        heldNotes.erase (std::remove (heldNotes.begin(), heldNotes.end(), note), heldNotes.end());
        heldNotes.push_back (note);
        Voice& v = voices[0];
        const bool wasGate = v.active && v.gate;
        const bool retrig = (prm.polyMode == 1) || ! wasGate;
        initVoice (v, note, vel, f, retrig);
    }
    lastFreq = f;
}

void AtlasRetroAudioProcessor::noteOff (int note)
{
    if (prm.polyMode == 0)
    {
        for (auto& v : voices)
            if (v.active && v.gate && v.note == note)
            {
                v.gate = false;
                v.env.noteOff();
            }
    }
    else
    {
        heldNotes.erase (std::remove (heldNotes.begin(), heldNotes.end(), note), heldNotes.end());
        Voice& v = voices[0];
        if (! heldNotes.empty())
        {
            const int n = heldNotes.back();
            v.note = n;
            v.target = 440.0f * std::exp2 (((float) n - 69.0f) / 12.0f);
            if (prm.polyMode == 1) { v.env.reset(); v.env.noteOn(); }
        }
        else if (v.active && v.gate)
        {
            v.gate = false;
            v.env.noteOff();
        }
    }
}

void AtlasRetroAudioProcessor::allNotesOff()
{
    heldNotes.clear();
    for (auto& v : voices)
        if (v.active && v.gate) { v.gate = false; v.env.noteOff(); }
}

void AtlasRetroAudioProcessor::handleMidi (const MidiMessage& m)
{
    if (m.isNoteOn())            noteOn (m.getNoteNumber(), m.getFloatVelocity());
    else if (m.isNoteOff())      noteOff (m.getNoteNumber());
    else if (m.isAllNotesOff() || m.isAllSoundOff()) allNotesOff();
    else if (m.isPitchWheel())   pitchBend.store ((float) (m.getPitchWheelValue() - 8192) / 8192.0f);
    else if (m.isController() && m.getControllerNumber() == 1)
        modWheel.store ((float) m.getControllerValue() / 127.0f);
}

//==============================================================================
void AtlasRetroAudioProcessor::renderVoices (float* L, float* R, int start, int n)
{
    const float invSr = 1.0f / (float) sr;
    const int nU = prm.unison;
    const float uniNorm = 1.0f / std::sqrt ((float) nU);
    const float vibAmt = prm.vibrato * modWheel.load() * 0.7f;
    const float bend = pitchBend.load() * 2.0f;
    const float glideCoef = prm.portamento > 0.001f
                              ? 1.0f - std::exp (-1.0f / (prm.portamento * 0.35f * (float) sr))
                              : 1.0f;
    const float gainBase = prm.volume * prm.volume * 1.6f * 0.25f;
    const float panA = (prm.pan + 1.0f) * MathConstants<float>::pi * 0.25f;
    const float gPL = std::cos (panA) * 1.4142f, gPR = std::sin (panA) * 1.4142f;
    const float k1 = 2.0f - 1.8f * prm.resonance;
    const float k2 = 1.4f;
    const float maxFc = std::min (18000.0f, (float) sr * 0.45f);
    const float twoPi = MathConstants<float>::twoPi;

    for (int i = 0; i < n; ++i)
    {
        lfoPhase += lfoHz * invSr;
        if (lfoPhase >= 1.0f) { lfoPhase -= 1.0f; lfoHold = rng.nextFloat() * 2.0f - 1.0f; }
        float lfoV = 0.0f;
        switch (prm.lfoWave)
        {
            case 0: lfoV = std::sin (lfoPhase * twoPi); break;
            case 1: lfoV = 1.0f - 4.0f * std::abs (lfoPhase - 0.5f); lfoV = -lfoV; break;
            case 2: lfoV = 2.0f * lfoPhase - 1.0f; break;
            case 3: lfoV = lfoPhase < 0.5f ? 1.0f : -1.0f; break;
            default: lfoV = lfoHold; break;
        }

        vibPhase += 5.5f * invSr;
        if (vibPhase >= 1.0f) vibPhase -= 1.0f;
        const float pitchMul = std::exp2 ((bend + std::sin (vibPhase * twoPi) * vibAmt) / 12.0f);

        float sumL = 0.0f, sumR = 0.0f;
        for (auto& v : voices)
        {
            if (! v.active) continue;
            v.freq += (v.target - v.freq) * glideCoef;

            const float env = v.env.getNextSample();
            if (! v.env.isActive()) { v.active = false; continue; }

            float vl = 0.0f, vr = 0.0f;
            for (int u = 0; u < nU; ++u)
            {
                const float f1 = v.freq * pitchMul * uniMul[u];
                const float inc1 = std::min (f1 * invSr, 0.45f);
                const float s1 = oscSample (prm.osc1Wave, v.ph1[u], inc1);
                v.ph1[u] += inc1;
                bool wrapped = false;
                if (v.ph1[u] >= 1.0f) { v.ph1[u] -= 1.0f; wrapped = true; }

                const float inc2 = std::min (f1 * prm.osc2Mul * invSr, 0.45f);
                if (prm.sync && wrapped) v.ph2[u] = 0.0f;
                const float s2 = oscSample (prm.osc2Wave, v.ph2[u], inc2);
                v.ph2[u] += inc2;
                if (v.ph2[u] >= 1.0f) v.ph2[u] -= 1.0f;

                const float s = s1 * prm.osc1Level + s2 * prm.osc2Level;
                vl += s * uniGL[u];
                vr += s * uniGR[u];
            }

            float fc = prm.cutoff * std::exp2 (lfoV * prm.lfoDepth * 3.0f + (env - 0.5f) * prm.filterEnv * 4.0f);
            fc = jlimit (20.0f, maxFc, fc);
            const float g = std::tan (MathConstants<float>::pi * fc * invSr);
            vl = runFilter (v.fa, prm.filtType, vl, g, k1, k2);
            vr = runFilter (v.fb, prm.filtType, vr, g, k1, k2);

            const float a = env * v.vel * uniNorm;
            sumL += vl * a;
            sumR += vr * a;
        }
        L[start + i] = sumL * gainBase * gPL;
        R[start + i] = sumR * gainBase * gPR;
    }
}

//==============================================================================
void AtlasRetroAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midi)
{
    ScopedNoDenormals noDenormals;
    buffer.clear();
    const int n = buffer.getNumSamples();
    if (buffer.getNumChannels() < 2 || n == 0) return;

    keyboardState.processNextMidiBuffer (midi, 0, n, true);
    updateParams();

    float* L = buffer.getWritePointer (0);
    float* R = buffer.getWritePointer (1);

    int cur = 0;
    for (const auto meta : midi)
    {
        const int pos = jlimit (0, n, meta.samplePosition);
        if (pos > cur) { renderVoices (L, R, cur, pos - cur); cur = pos; }
        handleMidi (meta.getMessage());
    }
    if (cur < n) renderVoices (L, R, cur, n - cur);

    // --- chorus ---
    chorus.setRate (prm.chorusRate);
    chorus.setDepth (prm.chorusDepth);
    chorus.setCentreDelay (7.0f);
    chorus.setFeedback (0.0f);
    chorus.setMix (prm.chorusDepth > 0.01f ? 0.5f : 0.0f);
    {
        dsp::AudioBlock<float> blk (buffer);
        dsp::ProcessContextReplacing<float> ctx (blk);
        chorus.process (ctx);
    }

    // --- delay ---
    {
        const int size = (int) delayBuf[0].size();
        const bool on = prm.delayTime > 0.02f;
        const int dt = jlimit (1, size - 2, (int) ((0.03f + prm.delayTime * 0.6f) * (float) sr));
        const float mix = on ? 0.35f : 0.0f;
        float* ch[2] = { L, R };
        for (int i = 0; i < n; ++i)
        {
            const int rp = (delayPos - dt + size) % size;
            for (int c = 0; c < 2; ++c)
            {
                const float in = ch[c][i];
                const float d = delayBuf[c][(size_t) rp];
                delayBuf[c][(size_t) delayPos] = in + d * prm.delayFb;
                ch[c][i] = in + d * mix;
            }
            delayPos = (delayPos + 1) % size;
        }
    }

    // --- reverb ---
    {
        Reverb::Parameters rp;
        rp.roomSize = 0.3f + prm.reverbSize * 0.65f;
        rp.damping = 0.45f;
        rp.wetLevel = prm.reverbMix * 0.45f;
        rp.dryLevel = 1.0f - prm.reverbMix * 0.3f;
        rp.width = 1.0f;
        rp.freezeMode = 0.0f;
        reverb.setParameters (rp);
        reverb.processStereo (L, R, n);
    }

    for (int i = 0; i < n; ++i)
    {
        L[i] = std::tanh (L[i]);
        R[i] = std::tanh (R[i]);
    }
    outLevel.store (buffer.getMagnitude (0, n));
}

//==============================================================================
void AtlasRetroAudioProcessor::loadPreset (int bank, int idx)
{
    bank = jlimit (0, AtlasPresets::numBanks - 1, bank);
    idx  = jlimit (0, AtlasPresets::presetsPerBank - 1, idx);
    const auto d = AtlasPresets::make (bank, idx);

    auto set = [this] (const char* id, float v)
    {
        if (auto* p = apvts.getParameter (id))
            p->setValueNotifyingHost (p->convertTo0to1 (v));
    };

    set ("osc1Wave", (float) d.osc1Wave);   set ("osc1Level", d.osc1Level);
    set ("osc2Wave", (float) d.osc2Wave);   set ("osc2Tune", d.osc2Tune);   set ("osc2Level", d.osc2Level);
    set ("sync", d.sync ? 1.f : 0.f);
    set ("cutoff", d.cutoff);               set ("resonance", d.resonance);
    set ("filterType", (float) d.filterType); set ("filterEnv", d.filterEnv);
    set ("attack", d.attack);               set ("decay", d.decay);
    set ("sustain", d.sustain);             set ("release", d.release);
    set ("lfoRate", d.lfoRate);             set ("lfoDepth", d.lfoDepth);
    set ("lfoWave", (float) d.lfoWave);     set ("lfoSync", d.lfoSync ? 1.f : 0.f);
    set ("pan", d.pan);                     set ("volume", d.volume);
    set ("polyphony", (float) d.polyMode);  set ("unison", (float) (d.unison - 1));
    set ("detune", d.detune);               set ("portamento", d.portamento);
    set ("vibrato", d.vibrato);
    set ("chorusDepth", d.chorusDepth);     set ("chorusRate", d.chorusRate);
    set ("delayTime", d.delayTime);         set ("delayFeedback", d.delayFb);
    set ("reverbSize", d.reverbSize);       set ("reverbMix", d.reverbMix);

    currentBank = bank;
    currentPreset = idx;
    sendChangeMessage();
}

void AtlasRetroAudioProcessor::toggleFav (int g)
{
    if (favs.count (g)) favs.erase (g); else favs.insert (g);
}

//==============================================================================
void AtlasRetroAudioProcessor::getStateInformation (MemoryBlock& dest)
{
    auto state = apvts.copyState();
    state.setProperty ("bank", currentBank, nullptr);
    state.setProperty ("preset", currentPreset, nullptr);
    StringArray f;
    for (int g : favs) f.add (String (g));
    state.setProperty ("favs", f.joinIntoString (","), nullptr);
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, dest);
}

void AtlasRetroAudioProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType()))
        {
            auto vt = ValueTree::fromXml (*xml);
            apvts.replaceState (vt);
            currentBank = jlimit (0, AtlasPresets::numBanks - 1, (int) vt.getProperty ("bank", 0));
            currentPreset = jlimit (0, AtlasPresets::presetsPerBank - 1, (int) vt.getProperty ("preset", 0));
            favs.clear();
            for (auto& s : StringArray::fromTokens (vt.getProperty ("favs", "").toString(), ",", ""))
                if (s.isNotEmpty()) favs.insert (s.getIntValue());
            sendChangeMessage();
        }
}

AudioProcessorEditor* AtlasRetroAudioProcessor::createEditor()
{
    return new AtlasRetroAudioProcessorEditor (*this);
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AtlasRetroAudioProcessor();
}
