#pragma once

#include "CoreMinimal.h"

struct FUTAToolCall
{
    FString ToolName;
    FString JsonArguments;
};

struct FUTAToolResult
{
    bool bSuccess = false;
    FString JsonPayload;
    FString ErrorMessage;
};

class IUTATool
{
public:
    virtual ~IUTATool() = default;

    virtual FString GetName() const = 0;
    virtual FUTAToolResult Execute(const FString& JsonArguments) = 0;
};

class IUTAToolRegistry
{
public:
    virtual ~IUTAToolRegistry() = default;

    virtual void RegisterTool(TSharedRef<IUTATool> Tool) = 0;
    virtual bool HasTool(const FString& ToolName) const = 0;
    virtual FUTAToolResult ExecuteTool(const FString& ToolName, const FString& JsonArguments) const = 0;
    virtual TArray<FString> ListTools() const = 0;
};

class IUTALLMProvider
{
public:
    virtual ~IUTALLMProvider() = default;

    virtual FString GetProviderId() const = 0;
    virtual void Configure(const FString& ApiKey, const FString& BaseUrl, const FString& Model) = 0;
    virtual bool GenerateResponse(const FString& UserPrompt, FString& OutResponse, FString& OutError) = 0;
};

class IUTAConversationStore
{
public:
    virtual ~IUTAConversationStore() = default;

    virtual void AppendUserMessage(const FString& Message) = 0;
    virtual void AppendAssistantMessage(const FString& Message) = 0;
    virtual TArray<FString> GetTranscript() const = 0;
};

class IUTAAgentOrchestrator
{
public:
    virtual ~IUTAAgentOrchestrator() = default;

    virtual bool ProcessUserMessage(const FString& Message, FString& OutAssistantResponse, FString& OutError) = 0;
};
