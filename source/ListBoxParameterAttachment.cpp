#include "ListBoxParameterAttachment.hpp"

#include <memory>

#include "juce_core/juce_core.h"



ListBoxParameterAttachment::ListBoxParameterAttachment (
                                juce::RangedAudioParameter& parameter, 
                                juce::ListBox& listBox,
                                juce::UndoManager* undoManager = nullptr) 
    : _storedParameter(parameter), 
      _lb(listBox),
      _attachment(parameter, [this] (float f) { setValue(f); }, undoManager)
{
    // we don't enable this.
    _lb.setMultipleSelectionEnabled(false);
    _cachedValue = _lb.getSelectedRow();
    _lb.addMouseListener(this, true);
    _lb.addKeyListener(this);
    sendInitialUpdate();
}

ListBoxParameterAttachment::~ListBoxParameterAttachment()
{
    _lb.removeMouseListener(this);
    _lb.removeKeyListener(this);
}

void ListBoxParameterAttachment::sendInitialUpdate()
{
_attachment.sendInitialUpdate();
}

void ListBoxParameterAttachment::setValue (float newValue)
{
    const auto normValue = _storedParameter.convertTo0to1 (newValue);
    const auto index = juce::roundToInt (normValue * (float) (_lb.getListBoxModel()->getNumRows() - 1));

    int sr = _lb.getSelectedRow();
    if (index == sr)
        return;

    const juce::ScopedValueSetter<bool> svs (_ignoreCallbacks, true);
    _lb.selectRow(index, false, true);
    _cachedValue = index;
}

void ListBoxParameterAttachment::valueMayHaveChanged()
{
    const auto selected = _lb.getSelectedRow();

    if (selected == _cachedValue) {
        return;
    }
    _cachedValue = selected;

    if (_ignoreCallbacks) 
        return;

    const auto numItems = _lb.getListBoxModel()->getNumRows();
    const auto newValue = numItems > 1 ? (float) selected / (float) (numItems - 1)
                                       : 0.0f;

    _attachment.setValueAsCompleteGesture (_storedParameter.convertFrom0to1 (newValue));    
}

std::unique_ptr<ListBoxParameterAttachment> ListBoxAttachment::makeAttachment (const juce::AudioProcessorValueTreeState& stateToUse,
                                                            const juce::String& parameterID,
                                                            juce::ListBox& control)
{
    if (auto* parameter = stateToUse.getParameter (parameterID))
        return std::make_unique<ListBoxParameterAttachment> (*parameter, control, stateToUse.undoManager);

    jassertfalse;
    return nullptr;
}

ListBoxAttachment::ListBoxAttachment (juce::AudioProcessorValueTreeState& stateToUse,
                        const juce::String& parameterID,
                        juce::ListBox& lb)
    : _attachment(makeAttachment(stateToUse, parameterID, lb))
{
}

