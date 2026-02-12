#include "SUTAChatPanel.h"

#include "Styling/AppStyle.h"
#include "UTAChatModule.h"
#include "UTAChatOrchestrator.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SVerticalBox.h"
#include "Widgets/Text/STextBlock.h"

void SUTAChatPanel::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SBorder)
        .BorderImage(FAppStyle::GetBrush("Brushes.Panel"))
        .Padding(6.0f)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight().Padding(4.0f)
            [
                SNew(STextBlock)
                .Text(FText::FromString(TEXT("Unreal Team Agents")))
                .Font(FAppStyle::Get().GetFontStyle("HeadingMedium"))
            ]
            + SVerticalBox::Slot().FillHeight(0.72f).Padding(4.0f)
            [
                SAssignNew(OutputText, SMultiLineEditableTextBox)
                .IsReadOnly(true)
            ]
            + SVerticalBox::Slot().FillHeight(0.2f).Padding(4.0f)
            [
                SAssignNew(InputText, SMultiLineEditableTextBox)
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(4.0f)
            [
                SNew(SButton)
                .ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("Button"))
                .Text(FText::FromString(TEXT("Send")))
                .OnClicked(this, &SUTAChatPanel::OnSendClicked)
            ]
        ]
    ];
}

FReply SUTAChatPanel::OnSendClicked()
{
    if (!InputText.IsValid())
    {
        return FReply::Handled();
    }

    const FString UserPrompt = InputText->GetText().ToString().TrimStartAndEnd();
    if (UserPrompt.IsEmpty())
    {
        return FReply::Handled();
    }

    AppendLine(FString::Printf(TEXT("user> %s"), *UserPrompt));

    FUTAChatModule* ChatModule = FModuleManager::GetModulePtr<FUTAChatModule>(TEXT("UTAChat"));
    if (!ChatModule)
    {
        AppendLine(TEXT("error> UTAChat module is not loaded"));
        return FReply::Handled();
    }

    TSharedPtr<FUTAChatOrchestrator> Orchestrator = ChatModule->GetOrchestrator();
    if (!Orchestrator.IsValid())
    {
        AppendLine(TEXT("error> Chat orchestrator is not initialized"));
        return FReply::Handled();
    }

    FString AssistantResponse;
    FString ErrorMessage;
    const bool bOk = Orchestrator->ProcessUserMessage(UserPrompt, AssistantResponse, ErrorMessage);

    if (!bOk)
    {
        AppendLine(FString::Printf(TEXT("error> %s"), *ErrorMessage));
    }
    else
    {
        AppendLine(FString::Printf(TEXT("uta> %s"), *AssistantResponse));
    }

    InputText->SetText(FText::GetEmpty());
    return FReply::Handled();
}

void SUTAChatPanel::AppendLine(const FString& Line) const
{
    if (!OutputText.IsValid())
    {
        return;
    }

    const FString Existing = OutputText->GetText().ToString();
    const FString Combined = Existing.IsEmpty() ? Line : Existing + LINE_TERMINATOR + Line;
    OutputText->SetText(FText::FromString(Combined));
}
