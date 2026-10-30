#include "PluginEditor.h"

using namespace juce;

namespace
{
const Colour cBg (0xff0a0a0b), cPanel (0xff101011), cLine (0xff3a3a3d),
             cText (0xffdcdcdc), cDim (0xff8e8e92), cBright (0xfff4f4f4);

void txt (Graphics& g, const String& s, Rectangle<int> r, float size, Justification j,
          Colour c, float kern = 0.08f, bool bold = false)
{
    Font f (size, bold ? Font::bold : Font::plain);
    f.setExtraKerningFactor (kern);
    g.setFont (f);
    g.setColour (c);
    g.drawText (s, r, j, false);
}

void panel (Graphics& g, Rectangle<int> r)
{
    auto f = r.toFloat();
    g.setColour (cPanel);
    g.fillRoundedRectangle (f, 5.0f);
    g.setColour (cLine);
    g.drawRoundedRectangle (f.reduced (0.5f), 5.0f, 1.2f);
}

void drawLogo (Graphics& g, Rectangle<float> r, Colour c)
{
    Path p;
    const float x = r.getX(), y = r.getY(), w = r.getWidth(), h = r.getHeight();
    p.startNewSubPath (x, y + h);
    p.lineTo (x + w * 0.5f, y);
    p.lineTo (x + w, y + h);
    p.lineTo (x + w * 0.74f, y + h);
    p.lineTo (x + w * 0.5f, y + h * 0.42f);
    p.lineTo (x + w * 0.26f, y + h);
    p.closeSubPath();
    g.setColour (c);
    g.fillPath (p);
}

void drawWaveIcon (Graphics& g, const String& name, Rectangle<float> r, Colour c)
{
    const float x = r.getX(), y = r.getY(), w = r.getWidth(), h = r.getHeight();
    Path p;
    if (name == "Saw")
    {
        p.startNewSubPath (x, y + h); p.lineTo (x + w * 0.5f, y); p.lineTo (x + w * 0.5f, y + h); p.lineTo (x + w, y);
    }
    else if (name == "Square")
    {
        p.startNewSubPath (x, y + h); p.lineTo (x, y); p.lineTo (x + w * 0.5f, y);
        p.lineTo (x + w * 0.5f, y + h); p.lineTo (x + w, y + h); p.lineTo (x + w, y);
    }
    else if (name == "Triangle")
    {
        p.startNewSubPath (x, y + h * 0.5f); p.lineTo (x + w * 0.25f, y);
        p.lineTo (x + w * 0.75f, y + h); p.lineTo (x + w, y + h * 0.5f);
    }
    else if (name == "Sine")
    {
        for (int i = 0; i <= 32; ++i)
        {
            const float t = (float) i / 32.0f;
            const float px = x + t * w, py = y + h * 0.5f - std::sin (t * MathConstants<float>::twoPi) * h * 0.5f;
            if (i == 0) p.startNewSubPath (px, py); else p.lineTo (px, py);
        }
    }
    else // S&H
    {
        const float ys[] = { 0.7f, 0.2f, 0.5f, 0.9f, 0.3f };
        p.startNewSubPath (x, y + h * ys[0]);
        for (int i = 0; i < 5; ++i)
        {
            p.lineTo (x + w * (float) (i + 1) / 5.0f, y + h * ys[i]);
            if (i < 4) p.lineTo (x + w * (float) (i + 1) / 5.0f, y + h * ys[i + 1]);
        }
    }
    g.setColour (c);
    g.strokePath (p, PathStrokeType (1.6f, PathStrokeType::curved, PathStrokeType::rounded));
}

void drawBankIcon (Graphics& g, int i, Rectangle<float> r, Colour c)
{
    const float x = r.getX(), y = r.getY(), w = r.getWidth(), h = r.getHeight();
    Path p;
    switch (i)
    {
        case 0: p.startNewSubPath (x, y + h * 0.6f); p.lineTo (x + w * 0.25f, y + h * 0.6f); p.lineTo (x + w * 0.4f, y);
                p.lineTo (x + w * 0.6f, y + h); p.lineTo (x + w * 0.75f, y + h * 0.4f); p.lineTo (x + w, y + h * 0.4f); break;
        case 1: for (int k = 0; k < 3; ++k) { p.startNewSubPath (x + w * 0.15f + k * w * 0.35f, y + h);
                                              p.lineTo (x + w * 0.15f + k * w * 0.35f, y + k * h * 0.25f); } break;
        case 2: p.addRectangle (x, y + h * 0.1f, w * 0.45f, h * 0.8f); p.addRectangle (x + w * 0.55f, y + h * 0.3f, w * 0.45f, h * 0.6f); break;
        case 3: p.startNewSubPath (x, y + h); p.lineTo (x, y + h * 0.66f); p.lineTo (x + w * 0.33f, y + h * 0.66f);
                p.lineTo (x + w * 0.33f, y + h * 0.33f); p.lineTo (x + w * 0.66f, y + h * 0.33f);
                p.lineTo (x + w * 0.66f, y); p.lineTo (x + w, y); break;
        case 4: p.addRoundedRectangle (x, y + h * 0.15f, w, h * 0.7f, 2.0f);
                for (int k = 1; k < 4; ++k) { p.startNewSubPath (x + w * k * 0.25f, y + h * 0.15f); p.lineTo (x + w * k * 0.25f, y + h * 0.85f); } break;
        case 5: p.startNewSubPath (x, y + h); p.lineTo (x + w * 0.3f, y); p.lineTo (x + w, y); p.lineTo (x + w * 0.7f, y + h); p.closeSubPath();
                p.startNewSubPath (x + w * 0.35f, y + h * 0.5f); p.lineTo (x + w * 0.85f, y + h * 0.5f); break;
        case 6: p.startNewSubPath (x, y + h); p.lineTo (x + w * 0.8f, y + h * 0.2f); p.addEllipse (x + w * 0.62f, y, w * 0.38f, w * 0.38f); break;
        case 7: p.startNewSubPath (x, y + h * 0.35f); p.lineTo (x + w * 0.5f, y + h * 0.35f); p.lineTo (x + w, y);
                p.lineTo (x + w, y + h); p.lineTo (x + w * 0.5f, y + h * 0.65f); p.lineTo (x, y + h * 0.65f); p.closeSubPath(); break;
        case 8: p.startNewSubPath (x, y + h); p.lineTo (x + w * 0.25f, y + h * 0.3f); p.lineTo (x + w * 0.5f, y + h * 0.8f);
                p.lineTo (x + w * 0.75f, y); p.lineTo (x + w, y + h * 0.6f); break;
        default: p.addEllipse (x, y, w, h); p.addEllipse (x + w * 0.3f, y + h * 0.3f, w * 0.4f, h * 0.4f); break;
    }
    g.setColour (c);
    g.strokePath (p, PathStrokeType (1.5f, PathStrokeType::curved, PathStrokeType::rounded));
}
} // namespace

//==============================================================================
class AtlasLnF : public LookAndFeel_V4
{
public:
    AtlasLnF()
    {
        setColour (ComboBox::textColourId, cText);
        setColour (PopupMenu::backgroundColourId, Colour (0xff141416));
        setColour (PopupMenu::textColourId, cText);
        setColour (PopupMenu::highlightedBackgroundColourId, Colour (0xff3a3a3d));
        setColour (PopupMenu::highlightedTextColourId, cBright);
        setColour (TextEditor::backgroundColourId, Colours::transparentBlack);
        setColour (TextEditor::textColourId, cText);
        setColour (TextEditor::outlineColourId, Colours::transparentBlack);
        setColour (TextEditor::focusedOutlineColourId, Colours::transparentBlack);
        setColour (ListBox::backgroundColourId, Colours::transparentBlack);
        setColour (ScrollBar::thumbColourId, Colour (0xff4a4a4e));
    }

    void drawRotarySlider (Graphics& g, int x, int y, int w, int h, float pos,
                           float startA, float endA, Slider&) override
    {
        auto b = Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (2.0f);
        const float d = jmin (b.getWidth(), b.getHeight());
        const auto c = b.getCentre();
        const float r = d * 0.5f;
        const float ang = startA + pos * (endA - startA);

        Path track;
        track.addCentredArc (c.x, c.y, r - 1.0f, r - 1.0f, 0.0f, startA, endA, true);
        g.setColour (Colour (0xff3a3a3d));
        g.strokePath (track, PathStrokeType (1.6f));

        Path val;
        val.addCentredArc (c.x, c.y, r - 1.0f, r - 1.0f, 0.0f, startA, ang, true);
        g.setColour (Colour (0xffd8d8d8));
        g.strokePath (val, PathStrokeType (1.8f));

        const float br = r - 6.0f;
        ColourGradient grad (Colour (0xff2c2c2f), c.x - br * 0.4f, c.y - br * 0.5f,
                             Colour (0xff0b0b0c), c.x + br * 0.5f, c.y + br * 0.7f, false);
        g.setGradientFill (grad);
        g.fillEllipse (c.x - br, c.y - br, br * 2.0f, br * 2.0f);
        g.setColour (Colour (0xff4a4a4e));
        g.drawEllipse (c.x - br, c.y - br, br * 2.0f, br * 2.0f, 1.0f);

        const float s = std::sin (ang), co = -std::cos (ang);
        g.setColour (cBright);
        g.drawLine (c.x + s * br * 0.3f, c.y + co * br * 0.3f, c.x + s * br * 0.88f, c.y + co * br * 0.88f, 2.0f);
    }

    void drawToggleButton (Graphics& g, ToggleButton& b, bool, bool) override
    {
        auto r = b.getLocalBounds().toFloat().reduced (1.0f);
        g.setColour (Colour (0xff0b0b0c));
        g.fillRoundedRectangle (r, r.getHeight() * 0.5f);
        g.setColour (cLine);
        g.drawRoundedRectangle (r, r.getHeight() * 0.5f, 1.0f);
        const float kd = r.getHeight() - 4.0f;
        const float kx = b.getToggleState() ? r.getRight() - kd - 2.0f : r.getX() + 2.0f;
        g.setColour (b.getToggleState() ? cBright : cDim);
        g.fillEllipse (kx, r.getY() + 2.0f, kd, kd);
    }

    void drawComboBox (Graphics& g, int w, int h, bool, int, int, int, int, ComboBox&) override
    {
        auto r = Rectangle<float> (0.0f, 0.0f, (float) w, (float) h).reduced (0.5f);
        g.setColour (Colour (0xff0e0e0f));
        g.fillRoundedRectangle (r, 4.0f);
        g.setColour (Colour (0xff3d3d40));
        g.drawRoundedRectangle (r, 4.0f, 1.0f);
        Path a;
        const float cx = (float) w - 14.0f, cy = (float) h * 0.5f;
        a.startNewSubPath (cx - 4.0f, cy - 2.0f); a.lineTo (cx, cy + 2.0f); a.lineTo (cx + 4.0f, cy - 2.0f);
        g.setColour (cDim);
        g.strokePath (a, PathStrokeType (1.4f));
    }

    Font getComboBoxFont (ComboBox&) override { return Font (15.0f); }

    void positionComboBoxText (ComboBox& box, Label& label) override
    {
        label.setBounds (8, 1, box.getWidth() - 26, box.getHeight() - 2);
        label.setFont (getComboBoxFont (box));
    }

    int getSliderThumbRadius (Slider&) override { return 9; }

    void drawLinearSlider (Graphics& g, int x, int y, int w, int h, float pos, float, float,
                           const Slider::SliderStyle style, Slider& s) override
    {
        if (style == Slider::LinearBar)
        {
            auto r = Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (0.5f);
            g.setColour (Colour (0xff0e0e0f));
            g.fillRoundedRectangle (r, 4.0f);
            g.setColour (Colour (0xff3d3d40));
            g.drawRoundedRectangle (r, 4.0f, 1.0f);
            g.setColour (Colour (0x30ffffff));
            const float frac = (float) s.valueToProportionOfLength (s.getValue());
            g.fillRoundedRectangle (r.withWidth (jmax (4.0f, r.getWidth() * frac)).reduced (1.5f), 3.0f);
            g.setColour (cText);
            g.setFont (Font (15.0f));
            g.drawText (s.getTextFromValue (s.getValue()), r.reduced (10.0f, 0.0f), Justification::centredLeft);
            return;
        }
        const float cx = (float) x + (float) w * 0.5f;
        g.setColour (Colour (0xff0b0b0c));
        g.fillRoundedRectangle (cx - 8.0f, (float) y, 16.0f, (float) h, 4.0f);
        g.setColour (cLine);
        g.drawRoundedRectangle (cx - 8.0f, (float) y, 16.0f, (float) h, 4.0f, 1.0f);
        Rectangle<float> th (cx - 11.0f, pos - 8.0f, 22.0f, 16.0f);
        g.setGradientFill (ColourGradient (Colour (0xff5a5a5e), th.getX(), th.getY(), Colour (0xff2a2a2c), th.getX(), th.getBottom(), false));
        g.fillRoundedRectangle (th, 3.0f);
        g.setColour (cBright.withAlpha (0.8f));
        g.drawLine (th.getX() + 3.0f, th.getCentreY(), th.getRight() - 3.0f, th.getCentreY(), 1.2f);
    }
};

//==============================================================================
class IconButton : public Component
{
public:
    enum Type { Star, Heart, Left, Right, Gear };
    explicit IconButton (Type t) : type (t) { setMouseCursor (MouseCursor::PointingHandCursor); }

    bool on = false;
    std::function<void()> onClick;

    void paint (Graphics& g) override
    {
        auto r = getLocalBounds().toFloat().reduced (3.0f);
        const auto c = r.getCentre();
        const float rad = jmin (r.getWidth(), r.getHeight()) * 0.5f;
        g.setColour ((hover || on) ? cBright : cDim);
        Path p;
        switch (type)
        {
            case Star:
                p.addStar (c, 5, rad * 0.45f, rad, 0.0f);
                if (on) g.fillPath (p); else g.strokePath (p, PathStrokeType (1.3f));
                break;
            case Heart:
                p.startNewSubPath (c.x, c.y + rad * 0.9f);
                p.cubicTo (c.x - rad * 1.6f, c.y - rad * 0.1f, c.x - rad * 0.7f, c.y - rad * 1.1f, c.x, c.y - rad * 0.35f);
                p.cubicTo (c.x + rad * 0.7f, c.y - rad * 1.1f, c.x + rad * 1.6f, c.y - rad * 0.1f, c.x, c.y + rad * 0.9f);
                p.closeSubPath();
                if (on) g.fillPath (p); else g.strokePath (p, PathStrokeType (1.3f));
                break;
            case Left:
                p.startNewSubPath (c.x + rad * 0.3f, c.y - rad * 0.6f); p.lineTo (c.x - rad * 0.3f, c.y); p.lineTo (c.x + rad * 0.3f, c.y + rad * 0.6f);
                g.strokePath (p, PathStrokeType (1.8f, PathStrokeType::curved, PathStrokeType::rounded));
                break;
            case Right:
                p.startNewSubPath (c.x - rad * 0.3f, c.y - rad * 0.6f); p.lineTo (c.x + rad * 0.3f, c.y); p.lineTo (c.x - rad * 0.3f, c.y + rad * 0.6f);
                g.strokePath (p, PathStrokeType (1.8f, PathStrokeType::curved, PathStrokeType::rounded));
                break;
            case Gear:
                p.addStar (c, 8, rad * 0.74f, rad, 0.0f);
                g.strokePath (p, PathStrokeType (1.4f));
                g.drawEllipse (c.x - rad * 0.32f, c.y - rad * 0.32f, rad * 0.64f, rad * 0.64f, 1.4f);
                break;
        }
    }
    void mouseEnter (const MouseEvent&) override { hover = true; repaint(); }
    void mouseExit (const MouseEvent&) override { hover = false; repaint(); }
    void mouseUp (const MouseEvent& e) override
    {
        if (getLocalBounds().contains (e.getPosition()) && onClick) onClick();
    }

private:
    Type type;
    bool hover = false;
};

//==============================================================================
class WaveBox : public Component
{
public:
    explicit WaveBox (RangedAudioParameter& p)
        : attach (p, [this] (float v) { idx = (int) v; repaint(); })
    {
        if (auto* cp = dynamic_cast<AudioParameterChoice*> (&p)) choices = cp->choices;
        setMouseCursor (MouseCursor::PointingHandCursor);
        attach.sendInitialUpdate();
    }

    void paint (Graphics& g) override
    {
        auto r = getLocalBounds().toFloat().reduced (0.5f);
        g.setColour (Colour (0xff0e0e0f));
        g.fillRoundedRectangle (r, 4.0f);
        g.setColour (Colour (0xff3d3d40));
        g.drawRoundedRectangle (r, 4.0f, 1.0f);
        if (isPositiveAndBelow (idx, choices.size()))
            drawWaveIcon (g, choices[idx], r.reduced (8.0f, 0.0f).withTrimmedBottom (8.0f).reduced (0.0f, 8.0f), cBright);
        const int n = choices.size();
        const float dotW = 4.0f, gap = 3.0f;
        float x0 = r.getCentreX() - ((float) n * dotW + (float) (n - 1) * gap) * 0.5f;
        for (int i = 0; i < n; ++i)
        {
            g.setColour (i == idx ? cBright : Colour (0xff4a4a4e));
            g.fillEllipse (x0 + (float) i * (dotW + gap), r.getBottom() - 8.0f, dotW, dotW);
        }
    }

    void mouseDown (const MouseEvent&) override
    {
        const int n = choices.size();
        if (n > 0) attach.setValueAsCompleteGesture ((float) ((idx + 1) % n));
    }

private:
    ParameterAttachment attach;
    StringArray choices;
    int idx = 0;
};

//==============================================================================
class WaveDisplay : public Component, private Timer
{
public:
    explicit WaveDisplay (AudioProcessorValueTreeState& a) : apvts (a) { startTimerHz (10); }

    void paint (Graphics& g) override
    {
        auto r = getLocalBounds().toFloat().reduced (0.5f);
        g.setColour (Colour (0xff0e0e0f));
        g.fillRoundedRectangle (r, 4.0f);
        g.setColour (Colour (0xff3d3d40));
        g.drawRoundedRectangle (r, 4.0f, 1.0f);

        auto v = [this] (const char* id) { return apvts.getRawParameterValue (id)->load(); };
        const int w1 = (int) v ("osc1Wave"), w2 = (int) v ("osc2Wave");
        const float l1 = v ("osc1Level"), l2 = v ("osc2Level");
        const float ratio = std::exp2 (v ("osc2Tune") / 12.0f);
        auto shape = [] (int w, float ph)
        {
            ph -= std::floor (ph);
            switch (w)
            {
                case 0: return 2.0f * ph - 1.0f;
                case 1: return ph < 0.5f ? 0.8f : -0.8f;
                case 2: return 4.0f * std::abs (ph - 0.5f) - 1.0f;
                default: return std::sin (ph * MathConstants<float>::twoPi);
            }
        };
        auto area = r.reduced (36.0f, 10.0f);
        Path p;
        const int N = (int) area.getWidth();
        for (int i = 0; i <= N; ++i)
        {
            const float t = (float) i / (float) N * 3.0f;
            float s = (shape (w1, t) * l1 + shape (w2, t * ratio) * l2) / (l1 + l2 + 0.001f);
            const float px = area.getX() + (float) i, py = area.getCentreY() - s * area.getHeight() * 0.5f;
            if (i == 0) p.startNewSubPath (px, py); else p.lineTo (px, py);
        }
        g.setColour (cText);
        g.strokePath (p, PathStrokeType (1.2f));
    }

private:
    void timerCallback() override { repaint(); }
    AudioProcessorValueTreeState& apvts;
};

class ReverbDisplay : public Component, private Timer
{
public:
    explicit ReverbDisplay (AudioProcessorValueTreeState& a) : apvts (a) { startTimerHz (10); }

    void paint (Graphics& g) override
    {
        auto r = getLocalBounds().toFloat().reduced (0.5f);
        g.setColour (Colour (0xff0e0e0f));
        g.fillRoundedRectangle (r, 4.0f);
        g.setColour (Colour (0xff3d3d40));
        g.drawRoundedRectangle (r, 4.0f, 1.0f);

        const float size = apvts.getRawParameterValue ("reverbSize")->load();
        const float mix = apvts.getRawParameterValue ("reverbMix")->load();
        auto area = r.reduced (10.0f, 8.0f).withTrimmedBottom (28.0f);
        g.setColour (cText);
        for (float px = 0.0f; px < area.getWidth(); px += 2.0f)
        {
            const float t = px / area.getWidth();
            const float env = std::exp (-t * (4.5f - 3.5f * size));
            const float n = std::abs (std::sin (px * 1.7f) * std::sin (px * 0.37f + 1.0f));
            const float a = (0.15f + 0.85f * n) * env * (0.25f + 0.75f * mix) * area.getHeight() * 0.5f;
            g.drawLine (area.getX() + px, area.getCentreY() - a, area.getX() + px, area.getCentreY() + a, 1.0f);
        }
        txt (g, "VINTAGE REVERB", getLocalBounds().removeFromBottom (30), 12.0f, Justification::centred, cText, 0.12f);
    }

private:
    void timerCallback() override { repaint(); }
    AudioProcessorValueTreeState& apvts;
};

class Meter : public Component, private Timer
{
public:
    explicit Meter (AtlasRetroAudioProcessor& p) : proc (p) { startTimerHz (30); }
    void paint (Graphics& g) override
    {
        const int segs = 12;
        const float h = (float) getHeight() / (float) segs;
        for (int i = 0; i < segs; ++i)
        {
            const bool lit = level > (float) i / (float) segs * 0.9f + 0.02f;
            g.setColour (lit ? Colour (0xfff0f0f0).withAlpha (0.45f + 0.55f * (float) i / (float) segs) : Colour (0xff1e1e20));
            g.fillRect ((float) 3, (float) getHeight() - (float) (i + 1) * h + 1.0f, (float) getWidth() - 6.0f, h - 2.0f);
        }
    }
private:
    void timerCallback() override
    {
        level = jmax (level * 0.85f, proc.outLevel.load());
        repaint();
    }
    AtlasRetroAudioProcessor& proc;
    float level = 0.0f;
};

class BankList : public Component
{
public:
    int selected = 0;
    std::function<void (int)> onSelect;

    Rectangle<float> rowBounds (int i) const { return { 6.0f, 6.0f + (float) i * 46.6f, (float) getWidth() - 12.0f, 42.0f }; }

    void paint (Graphics& g) override
    {
        for (int i = 0; i < AtlasPresets::numBanks; ++i)
        {
            auto rb = rowBounds (i);
            const bool sel = (i == selected);
            if (sel)
            {
                g.setGradientFill (ColourGradient (Colour (0xffe2e2e2), rb.getX(), rb.getY(), Colour (0xffb4b4b6), rb.getX(), rb.getBottom(), false));
                g.fillRoundedRectangle (rb, 3.0f);
            }
            else
            {
                g.setColour (Colour (0xff1f1f21));
                g.drawHorizontalLine ((int) rb.getBottom() + 2, rb.getX(), rb.getRight());
            }
            const Colour c = sel ? Colour (0xff111111) : Colour (0xffd0d0d0);
            drawBankIcon (g, i, Rectangle<float> (rb.getX() + 12.0f, rb.getCentreY() - 11.0f, 22.0f, 22.0f), c);
            auto tr = rb.toNearestInt();
            txt (g, String (i + 1).paddedLeft ('0', 2), tr.withTrimmedLeft (52), 15.0f, Justification::centredLeft, c, 0.05f);
            txt (g, AtlasPresets::bankNames[i], tr.withTrimmedLeft (90), 15.0f, Justification::centredLeft, c, 0.12f);
            if (sel)
            {
                Path a;
                a.startNewSubPath (rb.getRight() - 24.0f, rb.getCentreY() - 5.0f);
                a.lineTo (rb.getRight() - 18.0f, rb.getCentreY());
                a.lineTo (rb.getRight() - 24.0f, rb.getCentreY() + 5.0f);
                g.setColour (c);
                g.strokePath (a, PathStrokeType (1.8f));
            }
        }
    }

    void mouseDown (const MouseEvent& e) override
    {
        for (int i = 0; i < AtlasPresets::numBanks; ++i)
            if (rowBounds (i).contains (e.position) && onSelect) { onSelect (i); break; }
    }
};

//==============================================================================
struct AtlasRetroAudioProcessorEditor::Content : public Component,
                                                  private ListBoxModel,
                                                  private ChangeListener
{
    using SA = AudioProcessorValueTreeState::SliderAttachment;
    using CA = AudioProcessorValueTreeState::ComboBoxAttachment;
    using BA = AudioProcessorValueTreeState::ButtonAttachment;

    AtlasRetroAudioProcessor& proc;
    AtlasLnF lnf;

    BankList bankList;
    ListBox listBox { "presets", this };
    TextEditor search;
    ComboBox bankCombo, polyCombo, uniCombo, filterCombo;
    IconButton favStar { IconButton::Star }, heart { IconButton::Heart }, prevBtn { IconButton::Left },
               nextBtn { IconButton::Right }, cardNext { IconButton::Right }, wavePrev { IconButton::Left },
               waveNext { IconButton::Right }, gear { IconButton::Gear };

    Slider osc1Lvl, osc2Tune, cutoff, res, atk, dec, sus, rel, lfoRate, lfoDepth, fenv, pan, vol, detune,
           porta, portaBar, chDepth, chRate, dlTime, dlFb, rvSize, rvMix, vibrato, pitchS, modS;
    ToggleButton syncBtn, lfoSyncBtn;
    WaveBox osc1Wave, osc2Wave, lfoWave;
    WaveDisplay waveDisplay;
    ReverbDisplay revDisplay;
    Meter meter;
    MidiKeyboardComponent keyboard;

    std::vector<std::unique_ptr<SA>> sliderAtts;
    std::vector<std::unique_ptr<CA>> comboAtts;
    std::vector<std::unique_ptr<BA>> buttonAtts;

    std::vector<int> rows;
    bool favOnly = false;

    //--------------------------------------------------------------------------
    explicit Content (AtlasRetroAudioProcessor& p)
        : proc (p),
          osc1Wave (*p.apvts.getParameter ("osc1Wave")),
          osc2Wave (*p.apvts.getParameter ("osc2Wave")),
          lfoWave (*p.apvts.getParameter ("lfoWave")),
          waveDisplay (p.apvts), revDisplay (p.apvts), meter (p),
          keyboard (p.keyboardState, MidiKeyboardComponent::horizontalKeyboard)
    {
        setLookAndFeel (&lnf);

        // --- knobovi ---
        knob (osc1Lvl, "osc1Level", 134, 383, 62);
        knob (osc2Tune, "osc2Tune", 268, 383, 62);
        knob (cutoff, "cutoff", 98, 500, 70);
        knob (res, "resonance", 195, 500, 66);
        knob (atk, "attack", 88, 628, 48);
        knob (dec, "decay", 164, 628, 48);
        knob (sus, "sustain", 241, 628, 48);
        knob (rel, "release", 319, 628, 48);
        knob (lfoRate, "lfoRate", 86, 748, 48);
        knob (lfoDepth, "lfoDepth", 157, 748, 48);
        knob (fenv, "filterEnv", 268, 748, 40);
        knob (pan, "pan", 86, 860, 46);
        knob (vol, "volume", 159, 858, 54);
        knob (detune, "detune", 1263, 659, 54);
        knob (porta, "portamento", 1030, 788, 54);
        knob (chDepth, "chorusDepth", 1375, 195, 54);
        knob (chRate, "chorusRate", 1460, 195, 54);
        knob (dlTime, "delayTime", 1376, 348, 54);
        knob (dlFb, "delayFeedback", 1461, 348, 54);
        knob (rvSize, "reverbSize", 1377, 502, 54);
        knob (rvMix, "reverbMix", 1459, 502, 54);
        knob (vibrato, "vibrato", 1368, 788, 54);

        // --- waveform boxovi ---
        osc1Wave.setBounds (55, 358, 38, 54);   addAndMakeVisible (osc1Wave);
        osc2Wave.setBounds (190, 358, 38, 54);  addAndMakeVisible (osc2Wave);
        lfoWave.setBounds (198, 728, 46, 40);   addAndMakeVisible (lfoWave);

        // --- sync prekidaci ---
        syncBtn.setBounds (318, 380, 34, 18);
        lfoSyncBtn.setBounds (318, 740, 34, 18);
        addAndMakeVisible (syncBtn);
        addAndMakeVisible (lfoSyncBtn);
        buttonAtts.push_back (std::make_unique<BA> (proc.apvts, "sync", syncBtn));
        buttonAtts.push_back (std::make_unique<BA> (proc.apvts, "lfoSync", lfoSyncBtn));

        // --- comboboxi ---
        combo (filterCombo, "filterType", { 254, 508, 95, 32 });
        combo (polyCombo, "polyphony", { 998, 653, 122, 33 });
        combo (uniCombo, "unison", { 1152, 653, 62, 33 });

        portaBar.setSliderStyle (Slider::LinearBar);
        portaBar.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
        portaBar.setNumDecimalPlacesToDisplay (2);
        portaBar.setBounds (1107, 786, 185, 28);
        addAndMakeVisible (portaBar);
        sliderAtts.push_back (std::make_unique<SA> (proc.apvts, "portamento", portaBar));

        // --- display komponente ---
        waveDisplay.setBounds (995, 473, 301, 68);   addAndMakeVisible (waveDisplay);
        revDisplay.setBounds (1342, 574, 146, 113);  addAndMakeVisible (revDisplay);
        meter.setBounds (250, 825, 24, 78);          addAndMakeVisible (meter);

        // --- banke ---
        bankList.setBounds (385, 170, 200, 480);
        bankList.onSelect = [this] (int b) { if (b != proc.getBank()) proc.loadPreset (b, 0); };
        addAndMakeVisible (bankList);

        bankCombo.setBounds (726, 134, 200, 26);
        for (int i = 0; i < AtlasPresets::numBanks; ++i)
            bankCombo.addItem (String (i + 1).paddedLeft ('0', 2) + "  -  " + AtlasPresets::bankNames[i], i + 1);
        bankCombo.onChange = [this]
        {
            const int b = bankCombo.getSelectedItemIndex();
            if (b >= 0 && b != proc.getBank()) proc.loadPreset (b, 0);
        };
        addAndMakeVisible (bankCombo);

        // --- pretraga + lista ---
        search.setBounds (640, 179, 220, 30);
        search.setFont (Font (15.0f));
        search.setTextToShowWhenEmpty ("Search presets...", cDim);
        search.onTextChange = [this] { rebuildRows(); listBox.updateContent(); selectCurrentRow(); repaint(); };
        addAndMakeVisible (search);

        favStar.setBounds (929, 180, 28, 28);
        favStar.onClick = [this]
        {
            favOnly = ! favOnly;
            favStar.on = favOnly;
            favStar.repaint();
            rebuildRows(); listBox.updateContent(); selectCurrentRow(); repaint();
        };
        addAndMakeVisible (favStar);

        listBox.setBounds (603, 220, 350, 594);
        listBox.setRowHeight (29);
        listBox.setOutlineThickness (0);
        listBox.getVerticalScrollBar().setAutoHide (true);
        addAndMakeVisible (listBox);

        prevBtn.setBounds (696, 132, 26, 26);
        nextBtn.setBounds (936, 132, 26, 26);
        prevBtn.onClick = [this] { stepPreset (-1); };
        nextBtn.onClick = [this] { stepPreset (1); };
        addAndMakeVisible (prevBtn);
        addAndMakeVisible (nextBtn);

        // --- kartica ---
        heart.setBounds (1242, 136, 28, 28);
        heart.onClick = [this]
        {
            proc.toggleFav (proc.getBank() * AtlasPresets::presetsPerBank + proc.getPreset());
            heart.on = proc.isFav (proc.getBank() * AtlasPresets::presetsPerBank + proc.getPreset());
            repaint();
        };
        cardNext.setBounds (1278, 136, 28, 28);
        cardNext.onClick = [this] { stepPreset (1); };
        wavePrev.setBounds (997, 493, 28, 28);
        waveNext.setBounds (1266, 493, 28, 28);
        wavePrev.onClick = [this] { stepPreset (-1); };
        waveNext.onClick = [this] { stepPreset (1); };
        addAndMakeVisible (heart);
        addAndMakeVisible (cardNext);
        addAndMakeVisible (wavePrev);
        addAndMakeVisible (waveNext);

        gear.setBounds (1440, 40, 38, 38);
        gear.onClick = [this] { showMenu(); };
        addAndMakeVisible (gear);

        // --- pitch / mod kotaci ---
        pitchS.setSliderStyle (Slider::LinearVertical);
        pitchS.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
        pitchS.setRange (-1.0, 1.0, 0.0);
        pitchS.setValue (0.0);
        pitchS.setBounds (397, 860, 26, 80);
        pitchS.onValueChange = [this] { proc.pitchBend.store ((float) pitchS.getValue()); };
        pitchS.onDragEnd = [this] { pitchS.setValue (0.0, sendNotificationSync); };
        addAndMakeVisible (pitchS);

        modS.setSliderStyle (Slider::LinearVertical);
        modS.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
        modS.setRange (0.0, 1.0, 0.0);
        modS.setValue (0.0);
        modS.setBounds (441, 860, 26, 80);
        modS.onValueChange = [this] { proc.modWheel.store ((float) modS.getValue()); };
        addAndMakeVisible (modS);

        // --- klavijatura ---
        keyboard.setKeyWidth (18.6f);
        keyboard.setAvailableRange (36, 96);
        keyboard.setLowestVisibleKey (36);
        keyboard.setScrollButtonsVisible (false);
        keyboard.setColour (MidiKeyboardComponent::whiteNoteColourId, Colour (0xffe8e8e8));
        keyboard.setColour (MidiKeyboardComponent::blackNoteColourId, Colour (0xff0d0d0d));
        keyboard.setColour (MidiKeyboardComponent::keySeparatorLineColourId, Colour (0xff555555));
        keyboard.setColour (MidiKeyboardComponent::keyDownOverlayColourId, Colour (0x80909090));
        keyboard.setColour (MidiKeyboardComponent::mouseOverKeyOverlayColourId, Colour (0x30ffffff));
        keyboard.setColour (MidiKeyboardComponent::shadowColourId, Colour (0x00000000));
        keyboard.setBounds (492, 858, 672, 102);
        addAndMakeVisible (keyboard);

        proc.addChangeListener (this);
        refresh();
    }

    ~Content() override
    {
        proc.removeChangeListener (this);
        setLookAndFeel (nullptr);
    }

    //--------------------------------------------------------------------------
    void knob (Slider& s, const char* id, int cx, int cy, int d)
    {
        s.setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
        s.setRotaryParameters (MathConstants<float>::pi * 1.2f, MathConstants<float>::pi * 2.8f, true);
        s.setBounds (cx - d / 2, cy - d / 2, d, d);
        s.setPopupDisplayEnabled (true, true, this);
        addAndMakeVisible (s);
        sliderAtts.push_back (std::make_unique<SA> (proc.apvts, id, s));
        if (auto* prm = proc.apvts.getParameter (id))
            s.setDoubleClickReturnValue (true, prm->convertFrom0to1 (prm->getDefaultValue()));
    }

    void combo (ComboBox& c, const char* id, Rectangle<int> r)
    {
        if (auto* cp = dynamic_cast<AudioParameterChoice*> (proc.apvts.getParameter (id)))
            c.addItemList (cp->choices, 1);
        c.setBounds (r);
        addAndMakeVisible (c);
        comboAtts.push_back (std::make_unique<CA> (proc.apvts, id, c));
    }

    //--------------------------------------------------------------------------
    int globalIdx() const { return proc.getBank() * AtlasPresets::presetsPerBank + proc.getPreset(); }

    void stepPreset (int delta)
    {
        const int total = AtlasPresets::numBanks * AtlasPresets::presetsPerBank;
        const int g = (globalIdx() + delta + total) % total;
        proc.loadPreset (g / AtlasPresets::presetsPerBank, g % AtlasPresets::presetsPerBank);
    }

    void showMenu()
    {
        PopupMenu m;
        m.addItem (100, "Atlas Retro 1.0  -  by ProdByPerke", false, false);
        m.addSeparator();
        m.addItem (1, "Reload current preset");
        m.addItem (2, "Clear all favourites");
        m.showMenuAsync (PopupMenu::Options().withTargetComponent (&gear), [this] (int r)
        {
            if (r == 1) proc.loadPreset (proc.getBank(), proc.getPreset());
            if (r == 2) { proc.clearFavs(); refresh(); }
        });
    }

    void rebuildRows()
    {
        rows.clear();
        const auto q = search.getText().trim().toLowerCase();
        const int b = proc.getBank();
        for (int i = 0; i < AtlasPresets::presetsPerBank; ++i)
        {
            if (q.isNotEmpty() && ! String (AtlasPresets::presetNames[b][i]).toLowerCase().contains (q)) continue;
            if (favOnly && ! proc.isFav (b * AtlasPresets::presetsPerBank + i)) continue;
            rows.push_back (i);
        }
    }

    void selectCurrentRow()
    {
        const auto it = std::find (rows.begin(), rows.end(), proc.getPreset());
        if (it != rows.end()) listBox.selectRow ((int) (it - rows.begin()), false, true);
        else listBox.deselectAllRows();
    }

    void refresh()
    {
        bankList.selected = proc.getBank();
        bankList.repaint();
        bankCombo.setSelectedItemIndex (proc.getBank(), dontSendNotification);
        heart.on = proc.isFav (globalIdx());
        heart.repaint();
        rebuildRows();
        listBox.updateContent();
        selectCurrentRow();
        listBox.repaint();
        repaint();
    }

    void changeListenerCallback (ChangeBroadcaster*) override { refresh(); }

    //--------------------------------------------------------------------------
    int getNumRows() override { return (int) rows.size(); }

    void paintListBoxItem (int row, Graphics& g, int w, int h, bool sel) override
    {
        if (! isPositiveAndBelow (row, (int) rows.size())) return;
        const int pi = rows[(size_t) row];
        const int gi = proc.getBank() * AtlasPresets::presetsPerBank + pi;
        if (sel)
        {
            g.setColour (Colour (0xff2a2a2c));
            g.fillRoundedRectangle (1.0f, 1.0f, (float) w - 2.0f, (float) h - 2.0f, 3.0f);
            g.setColour (Colour (0xffb8b8ba));
            g.drawRoundedRectangle (1.0f, 1.0f, (float) w - 2.0f, (float) h - 2.0f, 3.0f, 1.0f);
        }
        else
        {
            g.setColour (Colour (0xff222224));
            g.drawHorizontalLine (h - 1, 6.0f, (float) w - 6.0f);
        }
        const Colour c = sel ? cBright : cText;
        txt (g, String (pi + 1).paddedLeft ('0', 2), { 14, 0, 40, h }, 14.0f, Justification::centredLeft, c, 0.05f);
        txt (g, AtlasPresets::presetNames[proc.getBank()][pi], { 62, 0, w - 110, h }, 15.0f, Justification::centredLeft, c, 0.05f);

        Path s;
        const float cx = (float) w - 24.0f, cy = (float) h * 0.5f;
        s.addStar ({ cx, cy }, 5, 3.6f, 8.0f, 0.0f);
        g.setColour (proc.isFav (gi) ? cBright : cDim);
        if (proc.isFav (gi)) g.fillPath (s); else g.strokePath (s, PathStrokeType (1.0f));
    }

    void listBoxItemClicked (int row, const MouseEvent& e) override
    {
        if (! isPositiveAndBelow (row, (int) rows.size())) return;
        if (e.x > listBox.getWidth() - 46)
        {
            const int gi = proc.getBank() * AtlasPresets::presetsPerBank + rows[(size_t) row];
            proc.toggleFav (gi);
            heart.on = proc.isFav (globalIdx());
            heart.repaint();
            listBox.repaint();
        }
    }

    void selectedRowsChanged (int last) override
    {
        if (! isPositiveAndBelow (last, (int) rows.size())) return;
        const int pi = rows[(size_t) last];
        if (pi != proc.getPreset()) proc.loadPreset (proc.getBank(), pi);
    }

    //--------------------------------------------------------------------------
    void label (Graphics& g, const String& s, int cx, int y, float size = 11.0f, int w = 100)
    {
        txt (g, s, { cx - w / 2, y, w, 16 }, size, Justification::centred, cText, 0.12f);
    }

    void paint (Graphics& g) override
    {
        g.fillAll (cBg);
        g.setColour (Colour (0xff2a2a2c));
        g.drawRoundedRectangle (2.0f, 2.0f, 1532.0f, 1020.0f, 14.0f, 3.0f);
        g.setColour (Colour (0xff343437));
        g.drawRect (28, 107, 1480, 897, 1);
        const Point<float> screws[] = { { 30.f, 30.f }, { 1506.f, 30.f }, { 30.f, 994.f }, { 1506.f, 994.f } };
        for (auto pt : screws)
        {
            g.setColour (Colour (0xff1a1a1c));
            g.fillEllipse (pt.x - 8.f, pt.y - 8.f, 16.f, 16.f);
            g.setColour (Colour (0xff4a4a4e));
            g.drawEllipse (pt.x - 8.f, pt.y - 8.f, 16.f, 16.f, 1.2f);
        }

        // ---------------- header ----------------
        drawLogo (g, { 58.0f, 26.0f, 76.0f, 68.0f }, cBright);
        txt (g, "ATLAS", { 172, 28, 280, 66 }, 52.0f, Justification::centredLeft, cBright, 0.9f);
        g.setColour (cLine);
        g.drawVerticalLine (465, 38.0f, 86.0f);
        txt (g, "RETRO", { 512, 34, 260, 52 }, 30.0f, Justification::centredLeft, cText, 1.3f);
        txt (g, "PRODBYPERKE", { 1100, 44, 200, 34 }, 15.0f, Justification::centredRight, cDim, 0.45f);
        g.setColour (cLine);
        g.drawVerticalLine (1322, 38.0f, 86.0f);
        txt (g, "VST3", { 1345, 44, 80, 34 }, 15.0f, Justification::centredLeft, cDim, 0.45f);

        // ---------------- leva kolona ----------------
        panel (g, { 42, 120, 322, 160 });
        txt (g, "ANALOG SOUNDS", { 62, 138, 200, 16 }, 12.0f, Justification::centredLeft, cText, 0.18f);
        txt (g, "VINTAGE VIBES", { 62, 156, 200, 16 }, 12.0f, Justification::centredLeft, cText, 0.18f);
        txt (g, "TIMELESS", { 62, 174, 200, 16 }, 12.0f, Justification::centredLeft, cText, 0.18f);
        g.setColour (cBright);
        g.drawHorizontalLine (203, 62.0f, 94.0f);
        drawMountains (g);

        panel (g, { 42, 295, 322, 130 });
        txt (g, "OSCILLATOR", { 55, 302, 200, 20 }, 14.0f, Justification::centredLeft, cText, 0.2f);
        txt (g, "OSC 1", { 55, 334, 60, 16 }, 11.0f, Justification::centredLeft, cText, 0.12f);
        txt (g, "OSC 2", { 190, 334, 60, 16 }, 11.0f, Justification::centredLeft, cText, 0.12f);
        g.setColour (cLine);
        g.drawVerticalLine (173, 330.0f, 418.0f);
        g.drawVerticalLine (303, 330.0f, 418.0f);
        txt (g, "SYNC", { 306, 358, 60, 16 }, 10.0f, Justification::centred, cText, 0.12f);
        txt (g, "OFF", { 306, 402, 30, 14 }, 8.0f, Justification::centred, cDim, 0.1f);
        txt (g, "ON", { 334, 402, 30, 14 }, 8.0f, Justification::centred, cDim, 0.1f);

        panel (g, { 42, 437, 322, 125 });
        txt (g, "FILTER", { 55, 444, 200, 20 }, 14.0f, Justification::centredLeft, cText, 0.2f);
        label (g, "CUTOFF", 98, 535);
        label (g, "RESONANCE", 195, 535);
        txt (g, "TYPE", { 256, 487, 60, 16 }, 10.0f, Justification::centredLeft, cText, 0.12f);

        panel (g, { 42, 572, 322, 108 });
        txt (g, "ENVELOPE", { 55, 579, 200, 20 }, 14.0f, Justification::centredLeft, cText, 0.2f);
        label (g, "ATTACK", 88, 656); label (g, "DECAY", 164, 656);
        label (g, "SUSTAIN", 241, 656); label (g, "RELEASE", 319, 656);

        panel (g, { 42, 690, 322, 105 });
        txt (g, "LFO", { 55, 697, 200, 20 }, 14.0f, Justification::centredLeft, cText, 0.2f);
        label (g, "RATE", 86, 774); label (g, "DEPTH", 157, 774);
        label (g, "WAVEFORM", 221, 774, 9.0f, 70);
        label (g, "F.ENV", 268, 774, 9.0f, 50);
        txt (g, "SYNC", { 306, 720, 60, 16 }, 10.0f, Justification::centred, cText, 0.12f);
        txt (g, "OFF", { 306, 760, 30, 14 }, 8.0f, Justification::centred, cDim, 0.1f);
        txt (g, "ON", { 334, 760, 30, 14 }, 8.0f, Justification::centred, cDim, 0.1f);

        panel (g, { 42, 805, 322, 112 });
        txt (g, "AMP", { 55, 812, 200, 20 }, 14.0f, Justification::centredLeft, cText, 0.2f);
        label (g, "PAN", 86, 890); label (g, "VOLUME", 159, 890);

        txt (g, "ATLAS RETRO", { 50, 938, 160, 20 }, 11.0f, Justification::centredLeft, cText, 0.25f);
        g.setColour (cLine);
        g.drawVerticalLine (180, 940.0f, 956.0f);
        txt (g, String (AtlasPresets::numBanks * AtlasPresets::presetsPerBank) + " VINTAGE PRESETS",
             { 208, 938, 160, 20 }, 11.0f, Justification::centredLeft, cText, 0.2f);

        // ---------------- banke ----------------
        panel (g, { 377, 120, 213, 706 });
        txt (g, "BANKS", { 399, 132, 120, 24 }, 14.0f, Justification::centredLeft, cText, 0.25f);
        g.setColour (cLine);
        g.drawHorizontalLine (166, 389.0f, 580.0f);

        // ---------------- preseti ----------------
        panel (g, { 591, 120, 376, 706 });
        txt (g, "PRESETS", { 609, 132, 100, 24 }, 14.0f, Justification::centredLeft, cText, 0.25f);
        auto sb = Rectangle<float> (603.0f, 177.0f, 352.0f, 34.0f);
        g.setColour (Colour (0xff0e0e0f));
        g.fillRoundedRectangle (sb, 4.0f);
        g.setColour (Colour (0xff3d3d40));
        g.drawRoundedRectangle (sb, 4.0f, 1.0f);
        g.setColour (cDim);
        g.drawEllipse (613.0f, 187.0f, 11.0f, 11.0f, 1.5f);
        g.drawLine (622.0f, 197.0f, 628.0f, 203.0f, 1.5f);
        txt (g, String ((int) rows.size()) + " / " + String (AtlasPresets::presetsPerBank),
             { 860, 177, 64, 34 }, 12.0f, Justification::centredRight, cDim, 0.05f);
        auto lb = Rectangle<float> (603.0f, 218.0f, 350.0f, 596.0f);
        g.setColour (Colour (0xff0d0d0e));
        g.fillRoundedRectangle (lb, 4.0f);
        g.setColour (Colour (0xff2d2d30));
        g.drawRoundedRectangle (lb, 4.0f, 1.0f);

        // ---------------- kartica ----------------
        const int b = proc.getBank(), pi = proc.getPreset();
        panel (g, { 979, 120, 332, 440 });
        txt (g, String (pi + 1).paddedLeft ('0', 2) + "  -  " + AtlasPresets::presetNames[b][pi],
             { 997, 134, 240, 32 }, 18.0f, Justification::centredLeft, cText, 0.08f);
        drawSynthArt (g, { 995, 174, 301, 141 });
        txt (g, AtlasPresets::presetNames[b][pi], { 1002, 326, 290, 30 }, 21.0f, Justification::centredLeft, cBright, 0.08f);
        g.setColour (cText);
        g.setFont (Font (15.5f));
        g.drawFittedText (AtlasPresets::description (b, pi), { 1002, 362, 280, 50 }, Justification::topLeft, 3);
        int tx = 1001;
        for (auto& tg : AtlasPresets::tags (b, pi))
        {
            Font f (11.5f); f.setExtraKerningFactor (0.1f);
            const int tw = (int) f.getStringWidthFloat (tg) + 24;
            g.setColour (Colour (0xff4a4a4e));
            g.drawRect (tx, 421, tw, 25, 1);
            txt (g, tg, { tx, 421, tw, 25 }, 11.5f, Justification::centred, cText, 0.1f);
            tx += tw + 14;
        }

        // ---------------- voice settings ----------------
        panel (g, { 979, 571, 332, 255 });
        txt (g, "VOICE SETTINGS", { 997, 580, 220, 22 }, 13.0f, Justification::centredLeft, cText, 0.2f);
        g.setColour (cLine);
        g.drawHorizontalLine (610, 991.0f, 1299.0f);
        txt (g, "POLYPHONY", { 998, 628, 130, 16 }, 10.0f, Justification::centredLeft, cText, 0.15f);
        txt (g, "UNISON", { 1152, 628, 80, 16 }, 10.0f, Justification::centredLeft, cText, 0.15f);
        label (g, "DETUNE", 1263, 690, 10.0f, 80);
        g.setColour (cLine);
        g.drawHorizontalLine (720, 991.0f, 1299.0f);
        txt (g, "PORTAMENTO", { 997, 732, 160, 20 }, 12.0f, Justification::centredLeft, cText, 0.18f);
        txt (g, "TIME", { 1108, 764, 60, 16 }, 10.0f, Justification::centredLeft, cText, 0.15f);

        // ---------------- desna kolona ----------------
        panel (g, { 1325, 120, 177, 706 });
        txt (g, "CHORUS", { 1341, 130, 120, 22 }, 13.0f, Justification::centredLeft, cText, 0.2f);
        label (g, "DEPTH", 1375, 228); label (g, "RATE", 1460, 228);
        g.setColour (cLine);
        g.drawHorizontalLine (270, 1337.0f, 1490.0f);
        txt (g, "DELAY", { 1341, 282, 120, 22 }, 13.0f, Justification::centredLeft, cText, 0.2f);
        label (g, "TIME", 1376, 380); label (g, "FEEDBACK", 1461, 380);
        g.drawHorizontalLine (422, 1337.0f, 1490.0f);
        txt (g, "REVERB", { 1341, 432, 120, 22 }, 13.0f, Justification::centredLeft, cText, 0.2f);
        label (g, "SIZE", 1377, 534); label (g, "MIX", 1459, 534);
        g.drawHorizontalLine (720, 1337.0f, 1490.0f);
        txt (g, "MOD WHEEL", { 1341, 730, 140, 22 }, 12.0f, Justification::centredLeft, cText, 0.18f);
        txt (g, "VIBRATO", { 1410, 782, 90, 16 }, 10.0f, Justification::centredLeft, cText, 0.15f);

        // ---------------- tipkovnica ----------------
        panel (g, { 377, 840, 810, 130 });
        txt (g, "PITCH", { 380, 940, 60, 16 }, 10.0f, Justification::centred, cText, 0.12f);
        txt (g, "MOD", { 424, 940, 60, 16 }, 10.0f, Justification::centred, cText, 0.12f);

        panel (g, { 1190, 840, 312, 130 });
        drawLogo (g, { 1208.0f, 872.0f, 56.0f, 52.0f }, cBright);
        txt (g, "ATLAS", { 1282, 878, 100, 22 }, 17.0f, Justification::centredLeft, cBright, 1.1f);
        txt (g, "RETRO", { 1286, 902, 100, 20 }, 13.0f, Justification::centredLeft, cText, 1.1f);
        g.setColour (cLine);
        g.drawVerticalLine (1391, 876.0f, 934.0f);
        txt (g, "PRODBYPERKE", { 1402, 890, 100, 20 }, 9.5f, Justification::centredLeft, cDim, 0.3f);
    }

    void drawMountains (Graphics& g)
    {
        const float mx = 52.0f, W = 300.0f, baseY = 268.0f;
        g.setColour (Colour (0x28ffffff));
        g.fillEllipse (284.0f, 152.0f, 34.0f, 34.0f);
        for (int r = 0; r < 9; ++r)
        {
            Path p;
            for (int i = 0; i < 40; ++i)
            {
                const float fi = (float) i;
                const float peak = std::exp (-std::pow ((fi - 24.0f) / 9.0f, 2.0f)) * 58.0f
                                 + std::exp (-std::pow ((fi - 9.0f) / 6.0f, 2.0f)) * 26.0f;
                const float wob = (std::sin (fi * 0.55f + (float) r * 0.8f) * 0.5f + 0.5f);
                const float hgt = (peak * (0.55f + 0.45f * wob)) * (1.0f - (float) r * 0.05f);
                const float px = mx + fi / 39.0f * W;
                const float py = baseY - (float) r * 5.5f - hgt;
                if (i == 0) p.startNewSubPath (px, py); else p.lineTo (px, py);
            }
            g.setColour (Colours::white.withAlpha (0.25f + 0.05f * (float) (8 - r)));
            g.strokePath (p, PathStrokeType (0.8f));
        }
    }

    void drawSynthArt (Graphics& g, Rectangle<int> r)
    {
        auto f = r.toFloat();
        g.setGradientFill (ColourGradient (Colour (0xff1a1a1c), f.getX(), f.getY(), Colour (0xff090909), f.getX(), f.getBottom(), false));
        g.fillRect (f);
        g.setColour (Colour (0xff3a3a3d));
        g.drawRect (f, 1.0f);
        // kontrolna ploca
        for (int i = 0; i < 14; ++i)
        {
            const float x = f.getX() + 14.0f + (float) i * 14.5f;
            g.setColour (Colour (0xff2b2b2e));
            g.fillRect (x, f.getY() + 24.0f, 3.0f, 40.0f);
            g.setColour (Colour (0xffc8c8c8));
            g.fillRect (x - 3.0f, f.getY() + 28.0f + (float) ((i * 7) % 24), 9.0f, 5.0f);
        }
        txt (g, "ATLAS RETRO", { r.getX() + 196, r.getY() + 14, 100, 20 }, 11.0f, Justification::centredRight, cText, 0.3f);
        for (int i = 0; i < 6; ++i)
        {
            g.setColour (Colour (0xff2b2b2e));
            g.drawEllipse (f.getX() + 218.0f + (float) (i % 3) * 22.0f, f.getY() + 40.0f + (float) (i / 3) * 22.0f, 12.0f, 12.0f, 1.0f);
        }
        // tipke
        const float ky = f.getY() + 84.0f, kh = f.getHeight() - 84.0f;
        const int nk = 28;
        const float kw = f.getWidth() / (float) nk;
        g.setColour (Colour (0xffc9c9c9));
        g.fillRect (f.getX(), ky, f.getWidth(), kh);
        g.setColour (Colour (0xff222222));
        for (int i = 0; i <= nk; ++i)
            g.drawVerticalLine ((int) (f.getX() + (float) i * kw), ky, ky + kh);
        for (int i = 0; i < nk - 1; ++i)
        {
            const int m = i % 7;
            if (m == 2 || m == 6) continue;
            g.fillRect (f.getX() + (float) (i + 1) * kw - kw * 0.3f, ky, kw * 0.6f, kh * 0.6f);
        }
    }
};

//==============================================================================
AtlasRetroAudioProcessorEditor::AtlasRetroAudioProcessorEditor (AtlasRetroAudioProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    content = std::make_unique<Content> (proc);
    addAndMakeVisible (*content);
    content->setBounds (0, 0, 1536, 1024);

    setResizable (true, true);
    setResizeLimits (768, 512, 1536, 1024);
    getConstrainer()->setFixedAspectRatio (1536.0 / 1024.0);
    setSize (1152, 768);
}

AtlasRetroAudioProcessorEditor::~AtlasRetroAudioProcessorEditor() = default;

void AtlasRetroAudioProcessorEditor::resized()
{
    if (content != nullptr)
    {
        content->setBounds (0, 0, 1536, 1024);
        content->setTransform (AffineTransform::scale ((float) getWidth() / 1536.0f));
    }
}
