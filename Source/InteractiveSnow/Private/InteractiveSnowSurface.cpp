// Originally made by Jose Ivan Lopez Romo (https://www.ivanlopezr.com)


#include "InteractiveSnowSurface.h"
#include "Components/StaticMeshComponent.h"
#include "InteractiveSnowComponent.h"


AInteractiveSnowSurface::AInteractiveSnowSurface(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	RootComponent = Mesh;

	InteractiveSnowComponent = CreateDefaultSubobject<UInteractiveSnowComponent>("Interactive Snow");
	AddOwnedComponent(InteractiveSnowComponent);
}

void AInteractiveSnowSurface::BeginPlay()
{
	Super::BeginPlay();
}

UInteractiveSnowComponent* AInteractiveSnowSurface::GetInteractiveSnowComponent() const
{
	return InteractiveSnowComponent;
}
