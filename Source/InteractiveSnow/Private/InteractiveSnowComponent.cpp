// Originally made by Jose Ivan Lopez Romo (https://www.ivanlopezr.com)


#include "InteractiveSnowComponent.h"
#include "Kismet/KismetRenderingLibrary.h"


const FName RENDER_TARGET_PARAMETER_NAME = "Displacement Map";
const FString NAME_SEPARATOR = TEXT("_");
const FString WARNING_HEADER = TEXT("WARNING :: [Interactive Snow Component] ::");


UInteractiveSnowComponent::UInteractiveSnowComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractiveSnowComponent::DrawMaterial(FVector2D UVs, UTexture2D* ShapeTexture, FVector2D TextureScale)
{
	if (!RenderTarget || !DrawMaterialInstance)
	{
		LogWarning("Either render target or the draw material instance is null. Unable to draw material on render target.");
		return;
	}

	if (CurrentShapeTexture != ShapeTexture)
	{
		DrawMaterialInstance->SetTextureParameterValue("Shape Texture", ShapeTexture);
		CurrentShapeTexture = ShapeTexture;
	}

	DrawMaterialInstance->SetScalarParameterValue("Scale X", TextureScale.X);
	DrawMaterialInstance->SetScalarParameterValue("Scale Y", TextureScale.Y);
	DrawMaterialInstance->SetVectorParameterValue("UV Location", FLinearColor(UVs.X, UVs.Y, 0.f, 1.f));

	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), RenderTarget, DrawMaterialInstance); // Actual render target
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), PrevRenderTarget, TextureCopyMaterialInstance); // Cached render target
}

void UInteractiveSnowComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = GetOwner(); // Need to delay this until BeginPlay so that it works when inherited by blueprints

	RenderTarget = CreateRenderTarget(RenderTargetResolution, ETextureRenderTargetFormat::RTF_R16f);
	PrevRenderTarget = CreateRenderTarget(RenderTargetResolution, ETextureRenderTargetFormat::RTF_R16f);

	InitMaterials();
}

UTextureRenderTarget2D* UInteractiveSnowComponent::CreateRenderTarget(int32 Resolution, ETextureRenderTargetFormat Format)
{
	UTextureRenderTarget2D* newRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, Resolution, Resolution, Format);
	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), newRenderTarget);

	return newRenderTarget;
}

void UInteractiveSnowComponent::InitMaterials()
{
	UStaticMeshComponent* meshComponent = Cast<UStaticMeshComponent>(OwnerActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));

	// Exit early if actor doesn't have a static mesh component 

	if (!meshComponent)
	{
		LogWarning("No mesh component found on actor: " + OwnerActor->GetActorLabel() + ". Unable to initialize material.");
		return;
	}

	// Use object's own material as base material if it is null

	if (!BaseMaterial)
	{
		BaseMaterial = meshComponent->GetMaterial(0);
	}

	// Create dynamic material and assign it to the static mesh component

	FString materialName = OwnerActor->GetName() + NAME_SEPARATOR + BaseMaterial->GetName();
	UMaterialInstanceDynamic* dynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this, FName(*materialName));
	dynamicMaterial->SetTextureParameterValue(RENDER_TARGET_PARAMETER_NAME, RenderTarget);

	meshComponent->SetMaterial(0, dynamicMaterial);

	// Create material for drawing on the render target as well

	if (!RenderTargetDrawMaterial)
	{
		LogWarning("Render target draw material is null. Unable to create dynamic material instance.");
		return;
	}

	FString renderTargetMaterialName = OwnerActor->GetName() + NAME_SEPARATOR + RenderTargetDrawMaterial->GetName();
	DrawMaterialInstance = UMaterialInstanceDynamic::Create(RenderTargetDrawMaterial, this, FName(*renderTargetMaterialName));
	DrawMaterialInstance->SetTextureParameterValue("PreviousRenderTexture", PrevRenderTarget);

	// Create copy render texture material

	if (!RenderTargetCopyMaterial)
	{
		LogWarning("Render target copy material is null. Unable to create dynamic material instance.");
		return;
	}

	FString copyMaterialName = OwnerActor->GetName() + NAME_SEPARATOR + RenderTargetCopyMaterial->GetName();
	TextureCopyMaterialInstance = UMaterialInstanceDynamic::Create(RenderTargetCopyMaterial, this, FName(*copyMaterialName));
	TextureCopyMaterialInstance->SetTextureParameterValue("TextureToCopy", RenderTarget);
}

void UInteractiveSnowComponent::LogWarning(FString Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s %s"), *WARNING_HEADER, *Message);
}
