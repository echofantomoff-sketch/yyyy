# ATLAS RETRO (by ProdByPerke)

Retro analogni sintisajzer, VST3 za Windows (JUCE / C++).

- 10 bankova (LEAD, PLUCK, PAD, BASS, KEYS, ORGAN, STRINGS, BRASS, ARP, VOICE)
- 20 presseta u svakom banku = 200 presseta
- Oscillator 1/2 (Saw/Square/Triangle/Sine, sync), filter (LP24, LP12, HP12, BP12),
  ADSR, LFO (sync), pan/volume, polyphony (Poly/Mono/Legato), unison 1-7, detune,
  portamento, chorus, delay, reverb, pitch/mod wheel, klavijatura
- Favoriti (srce / zvezdica), pretraga presseta, strelice za sledeci/prethodni preset

## Kako se pravi .exe installer (GitHub Actions)

1. Napravi novi repo na GitHub-u (npr. `atlas-retro`) i uploaduj SVE iz ovog foldera
   (ukljucujuci skriveni folder `.github`).
2. Otvori tab **Actions** -> workflow **Build Atlas Retro** (pokrece se sam na push,
   ili klikni *Run workflow*).
3. Kad zavrsi (zeleno), na dnu stranice u **Artifacts** skini **AtlasRetro-Installer**.
4. Raspakuj zip i pokreni `AtlasRetro_Setup.exe` -> Next -> Install.

Plugin se instalira u `C:\Program Files\Common Files\VST3\Atlas Retro.vst3`.

## FL Studio

Options -> Manage plugins -> **Find installed plugins** (ili Verify plugins).
Zatim: Add -> More plugins -> **Atlas Retro** (nalazi se pod Instruments / Synth).

## Fajlovi

- `Source/Presets.h` - imena svih 200 presseta + generisanje parametara po banku
- `Source/PluginProcessor.*` - sint engine, efekti, parametri, cuvanje stanja
- `Source/PluginEditor.*` - korisnicki interfejs prema dizajnu
- `installer/AtlasRetro.iss` - Inno Setup skripta za .exe installer
- `.github/workflows/build.yml` - automatski build
