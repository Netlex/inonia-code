#include "UTADeepSeekProvider.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "Serialization/JsonSerializer.h"

namespace
{
FString ExtractErrorMessage(const FString& ResponseBody)
{
    TSharedPtr<FJsonObject> JsonObject;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        return ResponseBody;
    }

    TSharedPtr<FJsonObject> ErrorObject;
    if (JsonObject->TryGetObjectField(TEXT("error"), ErrorObject) && ErrorObject.IsValid())
    {
        FString Message;
        if (ErrorObject->TryGetStringField(TEXT("message"), Message))
        {
            return Message;
        }
    }

    return ResponseBody;
}
} // namespace

FString FUTADeepSeekProvider::GetProviderId() const
{
    return TEXT("deepseek");
}

void FUTADeepSeekProvider::Configure(const FString& ApiKey, const FString& BaseUrl, const FString& Model)
{
    Settings.ApiKey = ApiKey;
    Settings.BaseUrl = BaseUrl;
    Settings.Model = Model;
}

void FUTADeepSeekProvider::Configure(const FUTAProviderSettings& InSettings)
{
    Settings = InSettings;
}

bool FUTADeepSeekProvider::GenerateResponse(const FString& UserPrompt, FString& OutResponse, FString& OutError)
{
    if (Settings.ApiKey.IsEmpty())
    {
        OutError = TEXT("DeepSeek API key is empty");
        return false;
    }

    const FString Url = Settings.BaseUrl + Settings.ChatEndpoint;

    TSharedPtr<FJsonObject> RequestJson = MakeShared<FJsonObject>();
    RequestJson->SetStringField(TEXT("model"), Settings.Model);

    TArray<TSharedPtr<FJsonValue>> Messages;
    {
        TSharedPtr<FJsonObject> UserMessage = MakeShared<FJsonObject>();
        UserMessage->SetStringField(TEXT("role"), TEXT("user"));
        UserMessage->SetStringField(TEXT("content"), UserPrompt);
        Messages.Add(MakeShared<FJsonValueObject>(UserMessage));
    }
    RequestJson->SetArrayField(TEXT("messages"), Messages);
    RequestJson->SetBoolField(TEXT("stream"), false);

    FString Body;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
    FJsonSerializer::Serialize(RequestJson.ToSharedRef(), Writer);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Settings.ApiKey));
    Request->SetTimeout(Settings.TimeoutSeconds);
    Request->SetContentAsString(Body);

    FEvent* HttpEvent = FPlatformProcess::GetSynchEventFromPool(true);

    bool bRequestCompleted = false;
    bool bRequestSucceeded = false;
    int32 ResponseCode = 0;
    FString ResponseBody;

    Request->OnProcessRequestComplete().BindLambda(
        [&](FHttpRequestPtr /*InRequest*/, FHttpResponsePtr InResponse, bool bConnectedSuccessfully)
        {
            bRequestCompleted = true;
            bRequestSucceeded = bConnectedSuccessfully && InResponse.IsValid() && EHttpResponseCodes::IsOk(InResponse->GetResponseCode());
            if (InResponse.IsValid())
            {
                ResponseCode = InResponse->GetResponseCode();
                ResponseBody = InResponse->GetContentAsString();
            }
            HttpEvent->Trigger();
        });

    if (!Request->ProcessRequest())
    {
        FPlatformProcess::ReturnSynchEventToPool(HttpEvent);
        OutError = TEXT("Failed to dispatch DeepSeek HTTP request");
        return false;
    }

    const bool bSignaled = HttpEvent->Wait(static_cast<uint32>(Settings.TimeoutSeconds * 1000.0f) + 1000u);
    FPlatformProcess::ReturnSynchEventToPool(HttpEvent);

    if (!bSignaled || !bRequestCompleted)
    {
        Request->CancelRequest();
        OutError = TEXT("DeepSeek request timed out");
        return false;
    }

    if (!bRequestSucceeded)
    {
        OutError = FString::Printf(TEXT("DeepSeek request failed. HTTP %d: %s"), ResponseCode, *ExtractErrorMessage(ResponseBody));
        return false;
    }

    TSharedPtr<FJsonObject> ResponseJson;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);
    if (!FJsonSerializer::Deserialize(Reader, ResponseJson) || !ResponseJson.IsValid())
    {
        OutError = TEXT("Failed to parse DeepSeek response JSON");
        return false;
    }

    const TArray<TSharedPtr<FJsonValue>>* Choices;
    if (!ResponseJson->TryGetArrayField(TEXT("choices"), Choices) || !Choices || Choices->Num() == 0)
    {
        OutError = TEXT("DeepSeek response does not contain choices");
        return false;
    }

    const TSharedPtr<FJsonObject> ChoiceObject = (*Choices)[0]->AsObject();
    if (!ChoiceObject.IsValid())
    {
        OutError = TEXT("DeepSeek choice payload is invalid");
        return false;
    }

    TSharedPtr<FJsonObject> MessageObject;
    if (!ChoiceObject->TryGetObjectField(TEXT("message"), MessageObject) || !MessageObject.IsValid())
    {
        OutError = TEXT("DeepSeek response missing message object");
        return false;
    }

    if (!MessageObject->TryGetStringField(TEXT("content"), OutResponse))
    {
        OutError = TEXT("DeepSeek response missing assistant content");
        return false;
    }

    return true;
}
