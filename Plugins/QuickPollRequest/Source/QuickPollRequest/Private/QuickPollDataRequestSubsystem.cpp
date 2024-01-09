// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickPollDataRequestSubsystem.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

void UQuickPollDataRequestSubsystem::OverrideServerEndpointConnection(const FString& NewServerEndpoint)
{
	ServerEndpoint = NewServerEndpoint;
}

void UQuickPollDataRequestSubsystem::RequestPollData(const FString& PollName)
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();

	Request->OnProcessRequestComplete().Unbind();
	
	Request->OnProcessRequestComplete().BindLambda
	(
		[this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool ConnectedSuccesfully)
		{
			TSharedPtr<FJsonObject> ResponseAsJSON;
			TSharedRef<TJsonReader<>> JSONReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			FJsonSerializer::Deserialize(JSONReader, ResponseAsJSON);

			FJsonObjectConverter::JsonObjectStringToUStruct(Response->GetContentAsString(), &LatestResults);


			
			OnReceivedQuickPollDataEvent.Broadcast(LatestResults);
		}
	);
	
	Request->SetURL(ServerEndpoint + PollName);
	Request->SetVerb("GET");
	Request->SetHeader("User-Agent", "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "application/json");
	Request->ProcessRequest();
}

bool UQuickPollDataRequestSubsystem::GetPollByName(const FString& PollName, FPoll& Poll)
{
	for( int i = 0; i < LatestResults.polls.Num(); ++i )
	{
		FPoll CurrentPoll = LatestResults.polls[i];
		if(CurrentPoll.title == PollName)
		{
			Poll = CurrentPoll;
			return true;
		}
	}

	return false;
}

bool UQuickPollDataRequestSubsystem::GetVotesForPollAndCategory(const FString& PollName, const FString& PollOption, int& Value)
{
	for(int PollIndex = 0; PollIndex < LatestResults.polls.Num(); ++PollIndex)
	{
		FPoll CurrentPoll = LatestResults.polls[PollIndex];
		if(true || CurrentPoll.title == PollName)
		{
			for(int PollOptions = 0; PollOptions < CurrentPoll.options.Num(); ++PollOptions)
			{
				FPollOptions CurrentOption = CurrentPoll.options[PollOptions];
				if(true || CurrentOption.label == PollOption)
				{
					Value = CurrentOption.votes; 
					return true;
				}
			}
		}
	}

	return false;
}
