#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Types/SlateEnums.h"

class SMultiLineEditableTextBox;
class SCheckBox;

class SUTAChatPanel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SUTAChatPanel)
    {
    }
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FReply OnSendClicked();
    void OnAutoRoutingChanged(ECheckBoxState NewState);
    ECheckBoxState GetAutoRoutingState() const;
    void AppendLine(const FString& Line) const;

    TSharedPtr<SMultiLineEditableTextBox> InputText;
    TSharedPtr<SMultiLineEditableTextBox> OutputText;
    TSharedPtr<SCheckBox> AutoRoutingCheckbox;
};
