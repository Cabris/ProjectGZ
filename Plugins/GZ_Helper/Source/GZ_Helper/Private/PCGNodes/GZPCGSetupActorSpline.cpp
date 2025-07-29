// Fill out your copyright notice in the Description page of Project Settings.

#include "PCGNodes/GZPCGSetupActorSpline.h"
#include "Async/Async.h"
#include "PCGComponent.h"
#include "PCGContext.h"
#include "PCGManagedResource.h"
#include "Data/PCGPointData.h"
#include "Data/PCGSpatialData.h"
#include "Components/SplineComponent.h"
#include "Helpers/PCGHelpers.h"
#define LOCTEXT_NAMESPACE "GZPCGSetupActorSplineElement"

namespace ConstValues
{
	const FName SplinePointsInputLabel = TEXT("SplinePointsIn");
	const FName SplinePointsOutputLabel = TEXT("SplinePointsOut");
	const FName ArriveTangentAttribute = TEXT("ArriveTangent");
	const FName LeaveTangentAttribute = TEXT("LeaveTangent");
}

typedef FPCGMetadataAttribute<FVector> TangentAttribute;

#if WITH_EDITOR
FName UGZPCGSetupActorSplineSettings::GetDefaultNodeName() const
{
	return FName(TEXT("GZPCGSetupActorSpline"));
}

FText UGZPCGSetupActorSplineSettings::GetDefaultNodeTitle() const
{
	return LOCTEXT("NodeTitle", "GZPCGSetupActorSpline");
}

FText UGZPCGSetupActorSplineSettings::GetNodeTooltipText() const
{
	return LOCTEXT("NodeTooltip", "Add tooltip here.");
}
#endif

TArray<FPCGPinProperties> UGZPCGSetupActorSplineSettings::InputPinProperties() const
{
	TArray<FPCGPinProperties> PinProperties;
	PinProperties.Emplace(ConstValues::SplinePointsInputLabel, // 指定pin名字
	                      EPCGDataType::Point, // 指定pin类型
	                      /*bInAllowMultipleConnections=*/true,
	                      /*bAllowMultipleData=*/true,
	                      LOCTEXT("TestPCGNodeInputPinTooltip", "TestPCGNode Input Pin Tooltip"));
	return PinProperties;
}

TArray<FPCGPinProperties> UGZPCGSetupActorSplineSettings::OutputPinProperties() const
{
	TArray<FPCGPinProperties> PinProperties;
	PinProperties.Emplace(ConstValues::SplinePointsOutputLabel, // 指定pin名字
	                      EPCGDataType::Point, // 指定pin类型
	                      /*bInAllowMultipleConnections=*/true,
	                      /*bAllowMultipleData=*/true,
	                      LOCTEXT("TestPCGNodeInputPinTooltip", "TestPCGNode Input Pin Tooltip"));
	return PinProperties;
}

FPCGElementPtr UGZPCGSetupActorSplineSettings::CreateElement() const
{
	return MakeShared<FGZPCGSetupActorSplineElement>();
}

bool FGZPCGSetupActorSplineElement::ExecuteInternal(FPCGContext* Context) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FGZPCGSetupActorSplineElement::Execute);

	check(Context);

	const UGZPCGSetupActorSplineSettings* Settings = Context->GetInputSettings<UGZPCGSetupActorSplineSettings>();
	check(Settings);
	AActor* TargetActor = Settings->TargetActor.Get();
	TArray<FPCGTaggedData> Inputs = Context->InputData.GetInputsByPin(ConstValues::SplinePointsInputLabel);

	if (TargetActor)
	{
		if (!Settings->AddActorTag.IsNone())
			TargetActor->Tags.AddUnique(Settings->AddActorTag);

		USplineComponent* TargetActorSplineComponent = TargetActor->FindComponentByClass<USplineComponent>();

		//TArray<FPCGTaggedData>& Outputs = Context->OutputData.TaggedData;

		for (const FPCGTaggedData& Input : Inputs)
		{
			const UPCGSpatialData* SpatialData = Cast<UPCGSpatialData>(Input.Data);
			if (!SpatialData)
			{
				UE_LOG(LogTemp, Error, TEXT("Invalid input data"));
				continue;
			}
			const UPCGPointData* PointData = SpatialData->ToPointData(Context);
			if (!PointData)
			{
				UE_LOG(LogTemp, Error, TEXT("Unable to get point data from input"));
				continue;
			}

			const UPCGMetadata* PointMetadata = PointData->ConstMetadata();
			check(PointMetadata);

			const auto ArriveTangentBaseAttribute = PointMetadata->GetConstAttribute(
				ConstValues::ArriveTangentAttribute);
			const auto LeaveTangentBaseAttribute = PointMetadata->GetConstAttribute(ConstValues::LeaveTangentAttribute);
			check(ArriveTangentBaseAttribute);
			check(LeaveTangentBaseAttribute);

			const auto ArriveTangentAttribute = static_cast<const TangentAttribute*>(ArriveTangentBaseAttribute);
			const auto LeaveTangentAttribute = static_cast<const TangentAttribute*>(LeaveTangentBaseAttribute);
			check(ArriveTangentAttribute);
			check(LeaveTangentAttribute);

			const FTransform SplineActorTransform = TargetActor->GetTransform();
			ESplinePointType::Type PointType = ESplinePointType::Curve;
			if (Settings->bLinear)
			{
				PointType = ESplinePointType::Linear;
			}

			const TArray<FPCGPoint>& Points = PointData->GetPoints();

			TArray<FSplinePoint> SplinePoints;
			SplinePoints.Reserve(Points.Num());

			for (int32 PointIndex = 0; PointIndex < Points.Num(); ++PointIndex)
			{
				const FPCGPoint& Point = Points[PointIndex];
				const FTransform& PointTransform = Point.Transform;
				const FVector LocalPosition =
					SplineActorTransform.InverseTransformPosition(PointTransform.GetLocation());

				const FVector LocalArriveTangent = ArriveTangentAttribute
					                                   ? SplineActorTransform.InverseTransformVector(
						                                   ArriveTangentAttribute->GetValueFromItemKey(
							                                   Point.MetadataEntry))
					                                   : FVector::ZeroVector;
				const FVector LocalLeaveTangent = LeaveTangentAttribute
					                                  ? SplineActorTransform.InverseTransformVector(
						                                  LeaveTangentAttribute->GetValueFromItemKey(
							                                  Point.MetadataEntry))
					                                  : FVector::ZeroVector;

				SplinePoints.Emplace(static_cast<float>(PointIndex), LocalPosition, LocalArriveTangent,
				                     LocalLeaveTangent,
				                     PointTransform.GetRotation().Rotator(),
				                     PointTransform.GetScale3D(),
				                     PointType);
			}

			if (Settings->bGenerateSplineComponentInTargetActor)
			{
				AsyncTask(ENamedThreads::GameThread, [TargetActor,ClosedLoop=Settings->bClosedLoop,
					          PCGComponent=Context->SourceComponent.Get(),
					          LocalSplinePoints=MoveTemp(SplinePoints) ]()
				          {
					          check(IsInGameThread());
					          USplineComponent* SplineComponent = NewObject<USplineComponent>(TargetActor);
					          SplineComponent->ComponentTags.Add(PCGComponent->GetFName());
					          SplineComponent->ComponentTags.Add(PCGHelpers::DefaultPCGTag);
					          SplineComponent->RegisterComponent();
					          TargetActor->AddInstanceComponent(SplineComponent);
					          SplineComponent->AttachToComponent(TargetActor->GetRootComponent(),
					                                             FAttachmentTransformRules(
						                                             EAttachmentRule::KeepRelative,
						                                             EAttachmentRule::KeepWorld,
						                                             EAttachmentRule::KeepWorld,
						                                             false));

					          SplineComponent->SetClosedLoop(ClosedLoop);
					          SplineComponent->AddPoints(LocalSplinePoints);

					          UPCGManagedComponent* ManagedComponent = NewObject<
						          UPCGManagedComponent>(PCGComponent);
					          ManagedComponent->GeneratedComponent = SplineComponent;
					          PCGComponent->AddToManagedResources(ManagedComponent);
				          });
			}
			else
			{
				if (TargetActorSplineComponent)
				{
					TargetActorSplineComponent->ClearSplinePoints(false);
					TargetActorSplineComponent->SetClosedLoop(Settings->bClosedLoop, false);
					TargetActorSplineComponent->AddPoints(SplinePoints);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Spline component not found"));
				}
			}
		}
	}

	//Context->OutputData = Context->InputData;
	FPCGDataCollection outputData;
	outputData.TaggedData = MoveTemp(Inputs);
	Context->OutputData = MoveTemp(outputData);
	return true;
}

#undef LOCTEXT_NAMESPACE
