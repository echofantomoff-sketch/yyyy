#pragma once
#include <JuceHeader.h>
#include <cmath>
#include <algorithm>

namespace AtlasPresets
{
constexpr int numBanks = 10;
constexpr int presetsPerBank = 20;

inline const char* const bankNames[numBanks] = {
    "LEAD", "PLUCK", "PAD", "BASS", "KEYS", "ORGAN", "STRINGS", "BRASS", "ARP", "VOICE" };

inline const char* const presetNames[numBanks][presetsPerBank] = {
    // 01 LEAD
    { "Classic Saw", "Vintage Lead", "Bright Future", "Analog Pulse", "Warm Square", "Fat Unison", "Acid Lead",
      "Retro Drive", "Soft Glide", "Digital Breeze", "Dark Lead", "Crystal", "Spacey", "Clean Phase", "Super Saw",
      "Sync Scream", "Solo Fire", "Moog Cry", "Neon Nights", "Glass Lead" },
    // 02 PLUCK
    { "Soft Pluck", "Analog Pluck", "Glass Pluck", "Rubber Pluck", "Bell Pluck", "Dry Pluck", "Wooden Pluck",
      "Retro Harp", "Kalimba Dream", "Mellow Pluck", "Funky Pluck", "Squelch Pluck", "Wire Pluck", "Dream Pluck",
      "Pizzicato Saw", "Short Stab", "Marimba Pluck", "Koto Synth", "Gentle Pluck", "Pop Pluck" },
    // 03 PAD
    { "Warm Pad", "Analog Pad", "Dream Pad", "Vintage Pad", "Soft Evolution", "Dark Pad", "Cloud Pad",
      "Glass Pad", "Ambient Bloom", "Sunrise Pad", "Slow Swell", "Nebula", "Tape Pad", "Cathedral", "Lush Pad",
      "Night Drive Pad", "Floating", "Velvet", "Aurora", "Deep Space" },
    // 04 BASS
    { "Sub Bass", "Analog Bass", "Fat Bass", "Moog Style Bass", "Round Bass", "Acid Bass", "Rubber Bass",
      "Wobble Bass", "Dirty Bass", "Funk Bass", "Deep Sub", "Pulse Bass", "Retro Bass", "Growl Bass", "Soft Bass",
      "Punch Bass", "Reese Bass", "Square Bass", "Tight Bass", "808 Style Bass" },
    // 05 KEYS
    { "Electric Keys", "Vintage Keys", "Soft Keys", "Bell Keys", "Warm Tine Keys", "Clav Style", "Glass Keys",
      "Dream Keys", "Dark Keys", "Bright Keys", "Chorus Keys", "Mellow Keys", "Digital Keys", "Tine Keys",
      "Pop Keys", "Lo-Fi Keys", "Wurly Style", "Music Box", "Studio Keys", "Late Night Keys" },
    // 06 ORGAN
    { "Classic Organ", "Jazz Organ", "Rock Organ", "Church Organ", "Soft Organ", "Percussive Organ",
      "Full Drawbars", "Combo Organ", "Vintage Organ", "Gospel Organ", "Pipe Organ", "Reed Organ", "Theater Organ",
      "Chorus Organ", "Dark Organ", "Bright Organ", "Spinning Organ", "Mellow Organ", "Surf Organ", "Hall Organ" },
    // 07 STRINGS
    { "Analog Strings", "String Ensemble", "Warm Strings", "Vintage Strings", "Slow Strings", "Soft Strings",
      "Bright Strings", "Cinema Strings", "Solina Style", "Mellow Strings", "Chorus Strings", "Dark Strings",
      "Tremolo Strings", "Lush Strings", "Orchestral Synth", "Super Strings", "Velvet Strings", "Dream Strings",
      "Epic Strings", "Silk Strings" },
    // 08 BRASS
    { "Analog Brass", "Brass Section", "Soft Brass", "Bright Brass", "Fat Brass", "Retro Brass", "Stab Brass",
      "Swell Brass", "Jump Brass", "Horn Lead", "Poly Brass", "Warm Brass", "Power Brass", "Funk Brass",
      "Mellow Brass", "Cinema Brass", "Sync Brass", "Fanfare", "Big Band Synth", "Hybrid Brass" },
    // 09 ARP
    { "Classic Arp", "Retro Arp", "Pulse Arp", "Bright Arp", "Soft Arp", "Gate Arp", "Dream Arp", "Dark Arp",
      "Trance Arp", "Bounce Arp", "Crystal Arp", "Warm Arp", "Sequence Arp", "Glass Arp", "Echo Arp",
      "Analog Seq", "Flicker Arp", "Night Arp", "Pop Arp", "Cosmic Arp" },
    // 10 VOICE
    { "Choir Pad", "Vocal Aah", "Vocal Ooh", "Analog Voices", "Ghost Choir", "Soft Choir", "Vowel Lead",
      "Formant Pad", "Robot Voice", "Breath Voice", "Angel Voice", "Dark Choir", "Vintage Choir", "Whisper Pad",
      "Talk Synth", "Vocoder Style", "Solo Voice", "Choir Swell", "Ethereal Voice", "Human Synth" }
};

struct PresetData
{
    int osc1Wave = 0, osc2Wave = 0, filterType = 0, polyMode = 0, unison = 1, lfoWave = 0;
    bool sync = false, lfoSync = false;
    float osc1Level = 0.8f, osc2Level = 0.6f, osc2Tune = 0.08f;
    float cutoff = 6000.f, resonance = 0.2f, filterEnv = 0.3f;
    float attack = 0.005f, decay = 0.4f, sustain = 0.8f, release = 0.25f;
    float lfoRate = 3.f, lfoDepth = 0.f, detune = 0.2f, portamento = 0.f, vibrato = 0.5f;
    float chorusDepth = 0.3f, chorusRate = 0.8f, delayTime = 0.2f, delayFb = 0.3f;
    float reverbSize = 0.5f, reverbMix = 0.2f, pan = 0.f, volume = 0.7f;
};

struct Range { float lo, hi; };
struct Tpl
{
    int w1[2]; int w2[2]; float o2[2];
    Range cutoff, res, fenv, att, dec, sus, rel, det, chorus, delay, dfb, rev, lfoD;
    int uni[2]; int poly[2]; int filt[2];
};

inline const Tpl& tpl (int b)
{
    static const Tpl t[numBanks] = {
        // LEAD
        { {0,1}, {0,1}, {0.08f,-0.1f}, {2500,12000}, {0.1f,0.45f}, {0.1f,0.5f}, {0.001f,0.02f}, {0.2f,0.8f}, {0.6f,0.95f},
          {0.15f,0.5f}, {0.1f,0.4f}, {0.2f,0.6f}, {0.1f,0.35f}, {0.2f,0.45f}, {0.1f,0.3f}, {0.0f,0.15f}, {1,3}, {1,2}, {0,1} },
        // PLUCK
        { {0,1}, {2,0}, {0.07f,12.0f}, {1500,8000}, {0.1f,0.5f}, {0.5f,1.0f}, {0.001f,0.004f}, {0.12f,0.5f}, {0.0f,0.12f},
          {0.1f,0.4f}, {0.05f,0.3f}, {0.0f,0.4f}, {0.1f,0.4f}, {0.2f,0.5f}, {0.1f,0.35f}, {0.0f,0.1f}, {1,2}, {0,0}, {0,1} },
        // PAD
        { {0,0}, {0,2}, {0.12f,-0.1f}, {800,6000}, {0.0f,0.25f}, {0.1f,0.4f}, {0.4f,2.5f}, {0.5f,2.0f}, {0.6f,0.9f},
          {1.0f,4.0f}, {0.3f,0.7f}, {0.4f,0.8f}, {0.0f,0.3f}, {0.2f,0.5f}, {0.4f,0.8f}, {0.05f,0.3f}, {3,6}, {0,0}, {0,1} },
        // BASS
        { {0,1}, {1,0}, {-12.0f,0.07f}, {200,1500}, {0.1f,0.6f}, {0.4f,0.9f}, {0.001f,0.005f}, {0.15f,0.5f}, {0.5f,0.9f},
          {0.05f,0.2f}, {0.0f,0.1f}, {0.0f,0.1f}, {0.0f,0.05f}, {0.0f,0.2f}, {0.0f,0.1f}, {0.0f,0.3f}, {1,1}, {1,2}, {0,0} },
        // KEYS
        { {2,3}, {1,3}, {12.0f,0.05f}, {2500,9000}, {0.0f,0.2f}, {0.2f,0.6f}, {0.001f,0.01f}, {0.4f,1.2f}, {0.3f,0.6f},
          {0.2f,0.6f}, {0.0f,0.2f}, {0.3f,0.7f}, {0.0f,0.2f}, {0.2f,0.4f}, {0.1f,0.3f}, {0.0f,0.05f}, {1,2}, {0,0}, {1,0} },
        // ORGAN
        { {3,1}, {3,2}, {12.0f,7.0f}, {5000,14000}, {0.0f,0.1f}, {0.0f,0.1f}, {0.005f,0.02f}, {0.1f,0.2f}, {0.9f,1.0f},
          {0.05f,0.15f}, {0.0f,0.15f}, {0.5f,0.9f}, {0.0f,0.1f}, {0.2f,0.3f}, {0.1f,0.3f}, {0.0f,0.05f}, {1,1}, {0,0}, {1,1} },
        // STRINGS
        { {0,0}, {0,0}, {0.1f,-0.1f}, {3000,9000}, {0.0f,0.15f}, {0.1f,0.3f}, {0.2f,0.8f}, {0.4f,1.0f}, {0.8f,1.0f},
          {0.5f,1.5f}, {0.2f,0.5f}, {0.5f,0.8f}, {0.0f,0.15f}, {0.2f,0.4f}, {0.3f,0.6f}, {0.05f,0.2f}, {3,5}, {0,0}, {0,1} },
        // BRASS
        { {0,0}, {0,1}, {0.1f,-0.08f}, {1000,5000}, {0.1f,0.3f}, {0.5f,0.9f}, {0.03f,0.12f}, {0.2f,0.5f}, {0.6f,0.8f},
          {0.1f,0.3f}, {0.1f,0.3f}, {0.1f,0.3f}, {0.0f,0.1f}, {0.2f,0.4f}, {0.1f,0.25f}, {0.0f,0.1f}, {2,3}, {0,0}, {0,1} },
        // ARP
        { {0,1}, {1,0}, {0.1f,12.0f}, {1500,7000}, {0.2f,0.5f}, {0.4f,0.8f}, {0.001f,0.003f}, {0.1f,0.3f}, {0.0f,0.3f},
          {0.05f,0.2f}, {0.05f,0.25f}, {0.1f,0.4f}, {0.3f,0.6f}, {0.3f,0.6f}, {0.1f,0.3f}, {0.2f,0.5f}, {1,2}, {0,0}, {0,1} },
        // VOICE
        { {2,0}, {3,2}, {12.0f,0.1f}, {1500,5000}, {0.3f,0.6f}, {0.2f,0.5f}, {0.05f,0.4f}, {0.3f,0.6f}, {0.6f,0.8f},
          {0.3f,1.0f}, {0.1f,0.3f}, {0.3f,0.6f}, {0.0f,0.2f}, {0.2f,0.4f}, {0.3f,0.6f}, {0.05f,0.25f}, {2,4}, {0,0}, {3,0} }
    };
    return t[b];
}

// deterministicki "random" 0..1 (isti preseti na svakom racunaru)
inline float rnd (int bank, int idx, int k)
{
    uint32_t x = (uint32_t) (bank * 7919 + idx * 104729 + k * 1299709 + 12345);
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    x *= 2654435761u; x ^= x >> 15; x *= 2246822519u; x ^= x >> 13;
    return (float) (x & 0xFFFFFF) / 16777216.0f;
}

inline PresetData make (int bank, int idx)
{
    const Tpl& t = tpl (bank);
    auto R   = [&] (int k) { return rnd (bank, idx, k); };
    auto lin = [&] (Range r, int k) { return r.lo + (r.hi - r.lo) * R (k); };
    auto lg  = [&] (Range r, int k)
    {
        const float lo = std::max (r.lo, 0.0005f), hi = std::max (r.hi, lo);
        return lo * std::pow (hi / lo, R (k));
    };

    PresetData d;
    d.osc1Wave   = t.w1[R (1) < 0.5f ? 0 : 1];
    d.osc2Wave   = t.w2[R (2) < 0.5f ? 0 : 1];
    d.osc2Tune   = t.o2[R (3) < 0.5f ? 0 : 1];
    d.osc1Level  = 0.7f + 0.2f * R (4);
    d.osc2Level  = 0.4f + 0.4f * R (5);
    d.cutoff     = lg (t.cutoff, 6);
    d.resonance  = lin (t.res, 7);
    d.filterEnv  = lin (t.fenv, 8);
    d.attack     = lg (t.att, 9);
    d.decay      = lg (t.dec, 10);
    d.sustain    = lin (t.sus, 11);
    d.release    = lg (t.rel, 12);
    d.detune     = lin (t.det, 13);
    d.chorusDepth= lin (t.chorus, 14);
    d.delayTime  = lin ({ 0.1f, 0.5f }, 15);
    d.delayFb    = lin (t.dfb, 16);
    d.reverbMix  = lin (t.rev, 17);
    d.lfoDepth   = lin (t.lfoD, 18);
    d.unison     = std::min (t.uni[1], t.uni[0] + (int) (R (19) * (float) (t.uni[1] - t.uni[0] + 1)));
    d.polyMode   = t.poly[R (20) < 0.5f ? 0 : 1];
    d.filterType = t.filt[R (21) < 0.5f ? 0 : 1];
    d.lfoRate    = 0.5f + 5.5f * R (22);
    d.lfoWave    = (int) (R (23) * 4.99f);
    d.reverbSize = 0.3f + 0.5f * R (24);
    d.chorusRate = 0.3f + 1.2f * R (25);
    d.portamento = (d.polyMode != 0 && R (26) < 0.5f) ? 0.02f + 0.12f * R (27) : 0.0f;
    d.lfoSync    = (bank == 8);
    d.vibrato    = 0.5f;

    // delay: arp/lead ga koriste, pad/bass/organ manje
    if (t.delay.hi < 0.15f) d.delayTime = lin (t.delay, 28);
    else if (bank == 8)     d.delayTime = 0.25f + 0.3f * R (28);
    else                    d.delayTime = (R (28) < 0.55f) ? 0.0f : 0.1f + 0.4f * R (29);

    // posebni slucajevi
    if (bank == 0 && idx == 0) // Classic Saw
    {
        d.osc1Wave = 0; d.osc2Wave = 0; d.osc2Tune = 0.09f; d.osc2Level = 0.7f; d.filterType = 0;
        d.cutoff = 5500.f; d.resonance = 0.2f; d.filterEnv = 0.3f;
        d.attack = 0.005f; d.decay = 0.4f; d.sustain = 0.8f; d.release = 0.25f;
        d.unison = 2; d.detune = 0.2f; d.chorusDepth = 0.4f; d.delayTime = 0.2f; d.reverbMix = 0.15f;
        d.polyMode = 1; d.portamento = 0.0f;
    }
    if ((bank == 0 && idx == 15) || (bank == 7 && idx == 16)) // Sync Scream / Sync Brass
    {
        d.sync = true; d.osc2Tune = 7.0f; d.osc2Wave = 0; d.resonance = 0.25f;
    }
    if (bank == 3 && idx == 10) { d.osc1Wave = 3; d.osc2Wave = 3; d.cutoff = 250.f; d.resonance = 0.0f; } // Deep Sub
    if (bank == 3 && idx == 0)  { d.osc1Wave = 3; d.osc2Wave = 3; d.cutoff = 400.f; d.resonance = 0.0f; } // Sub Bass
    return d;
}

inline juce::String description (int bank, int idx)
{
    static const char* adj[] = { "Warm", "Powerful", "Bright", "Smooth", "Rich", "Deep",
                                 "Airy", "Gritty", "Lush", "Soft", "Punchy", "Mellow" };
    static const char* noun[] = { "lead", "pluck", "pad", "bass", "keys", "organ", "string sound",
                                  "brass", "arpeggio", "voice" };
    int a = (int) (rnd (bank, idx, 40) * 12.f) % 12;
    int b = (int) (rnd (bank, idx, 41) * 12.f) % 12;
    if (b == a) b = (b + 1) % 12;
    return juce::String (adj[a]) + " and " + juce::String (adj[b]).toLowerCase() + " "
         + noun[bank] + " with analog character.";
}

inline juce::StringArray tags (int bank, int idx)
{
    static const char* tg[] = { "ANALOG", "VINTAGE", "WARM", "BRIGHT", "SOFT", "DARK", "LUSH", "FAT" };
    const int t1 = (int) (rnd (bank, idx, 42) * 8.f) % 8;
    const int t2 = (t1 + 1 + (int) (rnd (bank, idx, 43) * 7.f)) % 8;
    return juce::StringArray { bankNames[bank], tg[t1], tg[t2] };
}
} // namespace AtlasPresets
