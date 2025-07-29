// Fill out your copyright notice in the Description page of Project Settings.

#include "TestPCGNode.h"
#include "PCGContext.h"
#include "Data/PCGPointData.h"
#include "Data/PCGSpatialData.h"
#include "Metadata/PCGMetadataAccessor.h"

#define LOCTEXT_NAMESPACE "TestPCGNodeElement"

namespace TestPCGNodeSettings
{
	const FName NodeName = TEXT("TestPCGNode");
	const FName NodeTitle = TEXT("TestPCGNode");
	const FName InputLabel = TEXT("TestPCGNodeInput");
	const FName InputSplineLabel = TEXT("SplineInput");
	const FName ActorReferenceAttributeName = TEXT("ActorReference");
}

#if WITH_EDITOR
// The label the node is known as internally.
FName UTestPCGNodeSettings::GetDefaultNodeName() const
{
	return TestPCGNodeSettings::NodeName;
}

// Default node name shown in the graph editor. Include spaces.
FText UTestPCGNodeSettings::GetDefaultNodeTitle() const
{
	return LOCTEXT("NodeTitle", "TestPCGNode");
}

// Default tooltip for the node
FText UTestPCGNodeSettings::GetNodeTooltipText() const
{
	return LOCTEXT("NodeTooltip", "Add tooltip here.");
}
#endif //WITH_EDITOR

// Input/Output pin setup with specific properties, including:
// Pin data type, allowing singular or multiple inputs per pin, and creating multiple in/out pins.
TArray<FPCGPinProperties> UTestPCGNodeSettings::InputPinProperties() const
{
	//return Super::DefaultPointInputPinProperties();
	TArray<FPCGPinProperties> PinProperties;
	PinProperties.Emplace(TestPCGNodeSettings::InputLabel, // 指定pin名字
	                      EPCGDataType::Point, // 指定pin类型
	                      /*bInAllowMultipleConnections=*/true,
	                      /*bAllowMultipleData=*/true,
	                      LOCTEXT("TestPCGNodeInputPinTooltip", "TestPCGNode Input Pin Tooltip"));
	PinProperties.Emplace(TestPCGNodeSettings::InputSplineLabel, // 指定pin名字
	                      EPCGDataType::Spline, // 指定pin类型
	                      /*bInAllowMultipleConnections=*/true,
	                      /*bAllowMultipleData=*/true,
	                      LOCTEXT("TestPCGNodeInputPinTooltip", "TestPCGNode Input Pin Tooltip"));


	return PinProperties;
}

TArray<FPCGPinProperties> UTestPCGNodeSettings::OutputPinProperties() const
{
	return Super::DefaultPointOutputPinProperties();
	//return Super::OutputPinProperties();
}

// Creates the Element to be used for ExecuteInternal.
FPCGElementPtr UTestPCGNodeSettings::CreateElement() const
{
	return MakeShared<FTestPCGNodeElement>();
}

/*
* Processing function for this node. 
* Context holds the InputData, containing the input data collection for this node 
* and the OutputData, the output data collection to write to as output.
* Returns true if the processing is done. 
* Returning false will call back this function at next tick, and will call it until it returns true.
* Settings contains all the setup options for this node, and if a property was marked PCG_Overridable, 
* "Context->GetInputSettings" will contain the overridden value for this property if it is overridden
*/
bool FTestPCGNodeElement::ExecuteInternal(FPCGContext* Context) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FTestPCGNodeElement::Execute);

	check(Context);

	const UTestPCGNodeSettings* Settings = Context->GetInputSettings<UTestPCGNodeSettings>();
	check(Settings);
	const FPCGDataCollection& InputData = Context->InputData;
	Context->OutputData = InputData;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TEXT("Test Message"));
	UE_LOG(LogTemp, Warning, TEXT("FTestPCGNodeElement:ExecuteInternal"));

	//Get input by pin name
	TArray<FPCGTaggedData> Inputs = Context->InputData.GetInputsByPin(TestPCGNodeSettings::InputLabel);
	for (int i = 0; i < Inputs.Num(); i++)
	{
		//A pin can have multiple inputs
		const FPCGTaggedData& Input = Inputs[i];
		const UPCGSpatialData* InputSpatialData = Cast<UPCGSpatialData>(Input.Data);
		const UPCGPointData* PointData = InputSpatialData->ToPointData(Context);
		//Get points data array
		const TArray<FPCGPoint>& Points = PointData->GetPoints();
		const UPCGMetadata* Metadata = InputSpatialData->ConstMetadata();

		//Access Attriibute
		const FPCGMetadataAttributeBase* ActorReferenceBaseAttribute = Metadata->GetConstAttribute(TestPCGNodeSettings::ActorReferenceAttributeName);

		if (ActorReferenceBaseAttribute)
		{
			const FPCGMetadataAttribute<FSoftObjectPath>* ActorReferenceAttribute = static_cast<const FPCGMetadataAttribute<FSoftObjectPath>*>(
				ActorReferenceBaseAttribute);
			if (ActorReferenceAttribute)
			{
				for (int j = 0; j < Points.Num(); j++)
				{
					const FPCGPoint& Point = Points[j];
					const FTransform PointTransform = Point.Transform;
					const FString TransformString = PointTransform.ToString();

					const FSoftObjectPath SoftObjectPath = ActorReferenceAttribute->GetValueFromItemKey(
						Point.MetadataEntry);

					TSoftObjectPtr<AActor> SoftObjectPtr = TSoftObjectPtr<AActor>(SoftObjectPath);
					UE_LOG(LogTemp, Warning, TEXT("FTestPCGNodeElement:ExecuteInternal:SoftObjectPtr.IsValid %d" ), SoftObjectPtr.IsValid());
					if (SoftObjectPtr.IsValid())
					{
						AActor* Actor = SoftObjectPtr.LoadSynchronous();
						if (Actor)
						{
							auto Location = Actor->GetActorLocation();
							Location.Z = 4242;
							Actor->SetActorLocation(Location);
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("FTestPCGNodeElement:ExecuteInternal:SoftObjectPtr.LoadSynchronous Fail"));
						}
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("FTestPCGNodeElement:ExecuteInternal:SoftObjectPtr Is Not Valid"));
					}
					UE_LOG(LogTemp, Warning, TEXT("FTestPCGNodeElement:ExecuteInternal:TransformString: %s"), *TransformString);
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TransformString);
				}
			}
			else
			{
				UE_LOG(LogTemp, Error,
				       TEXT("FTestPCGNodeElement:ExecuteInternal:ActorReferenceAttribute Null"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error,
			       TEXT("FTestPCGNodeElement:ExecuteInternal:ActorReferenceBaseAttribute Null"));
		}
	}
	return true;
}

#undef LOCTEXT_NAMESPACE
