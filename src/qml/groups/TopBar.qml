import QtQuick 2.12
import UX 1.0
import RuntimeData 1.0
import SignalLinker 1.0
import "qrc:/qml/ui/buttons" as Buttons
import "qrc:/qml/ui/labels" as Labels
import "qrc:/qml/ui/dropdowns" as Dropdowns

Rectangle {
    id: base;
    height: 70;
    color: UX.primaryDark;
    
    FontLoader { id: fontRegular; source: "qrc:/fonts/SofiaSans-Regular.ttf" }
    FontLoader { id: fontMedium; source: "qrc:/fonts/SofiaSans-Medium.ttf" }
    FontLoader { id: fontSemiBold; source: "qrc:/fonts/SofiaSans-SemiBold.ttf" }
    FontLoader { id: fontBold; source: "qrc:/fonts/SofiaSans-Bold.ttf" }
    FontLoader { id: fontExtraBold; source: "qrc:/fonts/SofiaSans-ExtraBold.ttf" }

    Buttons.ClassicButton
    {
        id: formImageButton;
        anchors.bottom: parent.bottom;              anchors.bottomMargin: 14;
        anchors.left: parent.left;                  anchors.leftMargin: 8;

        display_mode: Buttons.ClassicButton.Mode.LabelOnly;
        fixed_width: 188;                           fixed_height: 28;
        label_text: RuntimeData.formingQueueMode === 0 ? "ФОРМИРОВАНИЕ РЛИ" : RuntimeData.formingContinuous ? "ОСТАНОВКА" : "ЗАПУСК ОЧЕРЕДИ";
        label_color: RuntimeData.formingContinuous ? UX.primaryDarker : UX.textWhite;          label_text_size: 17;        
        label_text_family: fontBold.name;   label_text_bold: true;
        background_color: RuntimeData.formingQueueMode === 0 ? UX.infoLight : RuntimeData.formingContinuous ? UX.textWhite : UX.accentLight;     
        background_secondary_color: Qt.lighter(background_color, 1.5); 
        background_radius: 2;
        onClicked: 
        { 
            if(!RuntimeData.formingContinuous) { 
                SignalLinker.formSingleImage(); 
                if(RuntimeData.formingQueueMode === 1)
                {
                    RuntimeData.formingContinuous = true; 
                }
            }
            else { RuntimeData.formingContinuous = false; }
        }
    }
    Labels.FramedLabel
    {
        id: formModeLabel;
        anchors.bottom: formImageButton.top;    anchors.bottomMargin: 4;
        anchors.left: formImageButton.left;

        fixed_width: 51;    fixed_height: 17;
        label_text: "РЕЖИМ";
        label_color: UX.textWhite;          label_text_size: 12;
        label_text_family: fontMedium.name; label_text_bold: true;
        label_textAlignment: Text.AlignHCenter;
        frame_radius: 2;                    frame_width: 1;
    }
    Dropdowns.Dropdown
    {
        id: formModeDropDown;
        anchors.left: formModeLabel.right;  anchors.leftMargin: 1;
        anchors.top: formModeLabel.top;
        
        fixed_width: 136;       fixed_height: 17;           fixed_drop: 17*2;
        label_text: RuntimeData.formingQueueMode === 0 ? "ОДИНОЧНОЕ" : "НЕПРЕРЫВНОЕ";
        label_color: UX.textWhite;          label_text_size: 12;
        label_text_family: fontMedium.name; label_text_bold: true;
        label_textAlignment: Text.AlignRight;
        highlight_color: UX.infoLight;
        frame_radius: 2;                    frame_width: 1;
        enabled: RuntimeData.formingContinuous ? false : true;
        container: Item {
            Buttons.LightButton
            {
                id: selectSingle;
                fixed_width: 136;               fixed_height: 17;
                label_text: "ОДИНОЧНОЕ";
                label_color: UX.primaryDarker;  label_text_size: 12;
                label_text_family: fontMedium.name;
                label_text_bold: true;         label_textAlignment: Text.AlignRight;
                highlight_color: UX.textFaded;
                frame_radius: 0;                frame_width: 1;
                frame_enabled: false;
                onClicked: {
                    formModeDropDown.shown = false;
                    RuntimeData.formingQueueMode = 0;
                }
            }
            Buttons.LightButton
            {
                id: selectContinuous;
                anchors.top: selectSingle.bottom;
                fixed_width: 136;               fixed_height: 17;
                label_text: "НЕПРЕРЫВНОЕ";
                label_color: UX.primaryDarker;  label_text_size: 12;
                label_text_family: fontMedium.name;
                label_text_bold: true;         label_textAlignment: Text.AlignRight;
                highlight_color: UX.textFaded;
                frame_radius: 0;                frame_width: 1;
                frame_enabled: false;
                onClicked: {
                    formModeDropDown.shown = false;
                    RuntimeData.formingQueueMode = 1;
                }
            }
        }
    }
}
