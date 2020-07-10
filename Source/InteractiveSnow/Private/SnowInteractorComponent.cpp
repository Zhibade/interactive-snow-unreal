// Originally made by Jose Ivan Lopez Romo (https://www.ivanlopezr.com)


#include "SnowInteractorComponent.h"
#include "InteractiveSnowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


constexpr float SCALE_FIXED_DISTANCE = 1.f; // 1 CM


USnowInteractorComponent::USnowInteractorComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = TickInterval;
}

void USnowInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector currentLocation = GetOwner()->GetActorLocation();

	FHitResult hit;
	FVector2D hitUVs;

	UInteractiveSnowComponent* snowComponent = GetSnowComponentUnderParent(hitUVs, hit);

	if (snowComponent && currentLocation != LastLocation)
	{
		int32 uvChannel = snowComponent->GetUsedUvChannel();

		FVector2D uvScale = GetHoleUvScale(HoleSize, hitUVs, uvChannel, hit);
		snowComponent->DrawMaterial(hitUVs, HoleTexture, uvScale, bIsActivePlayer);

		LastLocation = currentLocation;
	}
}

void USnowInteractorComponent::BeginPlay()
{
	Super::BeginPlay();
}

FVector2D USnowInteractorComponent::GetHoleUvScale(float SizeInCM, FVector2D TargetUVLocation, int32 UvChannel, const FHitResult& Hit) const
{
	// Use two other locations on the surface to determine scale

	FHitResult hitA = FHitResult(Hit);
	FHitResult hitB = FHitResult(Hit);

	// Get two random orthogonal points on the surface a fixed distance away from the original hit

	FVector randomDirection = UKismetMathLibrary::RandomUnitVector();
	FVector directionA = FVector::CrossProduct(randomDirection, Hit.Normal).GetSafeNormal();
	FVector directionB = FVector::CrossProduct(directionA, Hit.Normal).GetSafeNormal();
	
	hitA.Location = (directionA * SCALE_FIXED_DISTANCE) + Hit.Location;
	hitB.Location = (directionB * SCALE_FIXED_DISTANCE) + Hit.Location;

	// Get UVs of those points and find scaling factors for U and V

	FVector2D uvA;
	FVector2D uvB;
		
	UGameplayStatics::FindCollisionUV(hitA, UvChannel, uvA);
	UGameplayStatics::FindCollisionUV(hitB, UvChannel, uvB);

	FVector2D uvDiffA = uvA - TargetUVLocation;
	FVector2D uvDiffB = uvB - TargetUVLocation;

	// Find U axis projection of vector A, and V axis projection of vector B. This is to find the UV distortion of U axis in relation to the V axis.
	// Since vector A and B are orthogonal, the projections should be equal when there is no distortion. Otherwise we need to find the correction factor.
	// (U scale in relation to V = 1.0: no distortion, 2.0: U is scaled by 2x, 0.5: U is scaled by half of V, etc.)

	float projectionAonU = FVector2D::DotProduct(uvDiffA, FVector2D(1.f, 0.f));
	float projectionBonV = FVector2D::DotProduct(uvDiffB, FVector2D(0.f, 1.f));

	float uScale = FMath::Abs(projectionAonU / projectionBonV);

	// Find overall hole scale by removing distortion. The enlarged axis is corrected to prevent increasing the scale by mistakenly growing the smaller axis.
	// On the other hand, final scale value should always scale up to prevent making the hole shape smaller than the requested size.

	FVector2D finalScale = FVector2D::UnitVector;
	FVector2D correctedA = FVector2D(uvDiffA);

	if (uScale > 1.f)
	{
		correctedA.X = correctedA.X * (1.f / uScale);
		finalScale.X = uScale;
	}
	else
	{
		correctedA.Y = correctedA.Y * uScale;
		finalScale.Y = (1.f / uScale);
	}
	
	float overallScale = (SizeInCM / SCALE_FIXED_DISTANCE) * correctedA.Size();
	finalScale *= overallScale;

	return finalScale;
}

UInteractiveSnowComponent* USnowInteractorComponent::GetSnowComponentUnderParent(FVector2D& OutUVs, FHitResult& Hit) const
{
	UInteractiveSnowComponent* foundComponent = nullptr;

	FVector start = GetOwner()->GetActorLocation();
	FVector end = FVector(start.X, start.Y, start.Z - MaxDistance);

	// Need complex trace to get UVs
	FCollisionQueryParams params = FCollisionQueryParams::DefaultQueryParam;
	params.bReturnFaceIndex = true;
	params.bTraceComplex = true;

	if (GetWorld()->LineTraceSingleByChannel(Hit, start, end, ECollisionChannel::ECC_Visibility, params)) {
		foundComponent = Cast<UInteractiveSnowComponent>(Hit.Actor->GetComponentByClass(UInteractiveSnowComponent::StaticClass()));

		int32 uvChannel = 0;
		if (foundComponent)
		{
			uvChannel = foundComponent->GetUsedUvChannel();
		}

		UGameplayStatics::FindCollisionUV(Hit, uvChannel, OutUVs);
	}

	return foundComponent;
}

