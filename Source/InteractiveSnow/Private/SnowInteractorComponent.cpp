// Originally made by Jose Ivan Lopez Romo (https://www.ivanlopezr.com)


#include "SnowInteractorComponent.h"
#include "InteractiveSnowComponent.h"
#include "Kismet/GameplayStatics.h"


USnowInteractorComponent::USnowInteractorComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = TickInterval;
}

void USnowInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector currentLocation = GetOwner()->GetActorLocation();

	FVector2D hitUVs;
	UInteractiveSnowComponent* snowComponent = GetSnowComponentUnderParent(hitUVs);

	if (snowComponent && currentLocation != LastLocation)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *hitUVs.ToString());
		snowComponent->DrawMaterial(hitUVs, HoleTexture, HoleScale);
		LastLocation = currentLocation;
	}
}

void USnowInteractorComponent::BeginPlay()
{
	Super::BeginPlay();
}

UInteractiveSnowComponent* USnowInteractorComponent::GetSnowComponentUnderParent(FVector2D& OutUVs) const
{
	UInteractiveSnowComponent* foundComponent = nullptr;

	FVector start = GetOwner()->GetActorLocation();
	FVector end = FVector(start.X, start.Y, start.Z - MaxDistance);

	FHitResult hit;

	// Need complex trace to get UVs
	FCollisionQueryParams params = FCollisionQueryParams::DefaultQueryParam;
	params.bReturnFaceIndex = true;
	params.bTraceComplex = true;

	if (GetWorld()->LineTraceSingleByChannel(hit, start, end, ECollisionChannel::ECC_Visibility, params)) {
		foundComponent = Cast<UInteractiveSnowComponent>(hit.Actor->GetComponentByClass(UInteractiveSnowComponent::StaticClass()));
		UGameplayStatics::FindCollisionUV(hit, 0, OutUVs);
	}

	return foundComponent;
}

