#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SMultiLineEditableTextBox;

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
    void AppendLine(const FString& Line) const;

    TSharedPtr<SMultiLineEditableTextBox> InputText;
    TSharedPtr<SMultiLineEditableTextBox> OutputText;
};
