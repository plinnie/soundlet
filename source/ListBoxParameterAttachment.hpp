#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_audio_processors/juce_audio_processors.h"

class ListBoxParameterAttachment : private juce::KeyListener, juce::MouseListener
{
public:
    ListBoxParameterAttachment (
        juce::RangedAudioParameter& parameter, 
        juce::ListBox& lb,
        juce::UndoManager* undoManager);    
    
    ~ListBoxParameterAttachment() override;

    void sendInitialUpdate();

    bool keyPressed (const juce::KeyPress& key,
                             juce::Component* originatingComponent) override 
    {
        valueMayHaveChanged();
        return false;
    }

    bool keyStateChanged (bool isKeyDown, juce::Component* originatingComponent)
    {
        valueMayHaveChanged();
        return false;
    }

    void mouseDown (const juce::MouseEvent& event) override
    {
        valueMayHaveChanged();
    }

private:
    void setValue (float newValue);
    void valueMayHaveChanged();

    juce::ListBox& _lb;
    juce::RangedAudioParameter& _storedParameter;
    juce::ParameterAttachment _attachment;
    bool _ignoreCallbacks = false;
    int _cachedValue;
};

class ListBoxAttachment
{
public:
    ListBoxAttachment (juce::AudioProcessorValueTreeState& stateToUse,
                        const juce::String& parameterID,
                        juce::ListBox& lb);

private:
    std::unique_ptr<ListBoxParameterAttachment> makeAttachment (const juce::AudioProcessorValueTreeState& stateToUse,
                                                                const juce::String& parameterID,
                                                                juce::ListBox& control);

    std::unique_ptr<ListBoxParameterAttachment> _attachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ListBoxAttachment)
};
