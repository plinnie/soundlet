function(target_cool_gui target)

    set(COOL_GUI_DIR ${CMAKE_CURRENT_FUNCTION_LIST_DIR})

    target_sources(${target}
        INTERFACE
        ${COOL_GUI_DIR}/src/CoolButton.cpp
        ${COOL_GUI_DIR}/src/CoolKnob.cpp
        ${COOL_GUI_DIR}/src/CoolSlider.cpp
        ${COOL_GUI_DIR}/src/CoolLookAndFeel.cpp
        ${COOL_GUI_DIR}/src/CoolPanel.cpp
    )

    juce_add_binary_data(CoolGuiData
        HEADER_NAME CoolGuiData.hpp
        NAMESPACE CoolGuiData
        SOURCES
        ${COOL_GUI_DIR}/res/rotational_blue_modern.png
        ${COOL_GUI_DIR}/res/switch_blue_modern.png
        ${COOL_GUI_DIR}/res/square_knob_modern.png
        ${COOL_GUI_DIR}/res/PastiOblique.otf
        ${COOL_GUI_DIR}/res/PastiRegular.otf
    )

    target_include_directories(${target} INTERFACE ${COOL_GUI_DIR}/src)

    target_link_libraries(${target} INTERFACE CoolGuiData)
endfunction()
