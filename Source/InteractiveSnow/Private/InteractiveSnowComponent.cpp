// Originally made by Jose Ivan Lopez Romo (https://www.ivanlopezr.com)


#include "InteractiveSnowComponent.h"
#include "Kismet/KismetRenderingLibrary.h"


const FName LOCATION_PARAMETER_NAME = "UV Location";
const FName SCALE_X_PARAMETER_NAME = "Scale X";
const FName SCALE_Y_PARAMETER_NAME = "Scale Y";
const FName PREV_OFFSET_X_PARAMETER_NAME = "Previous Texture Offset X";
const FName PREV_OFFSET_Y_PARAMETER_NAME = "Previous Texture Offset Y";
const FName RENDER_TARGET_PARAMETER_NAME = "Displacement Map";

const FString NAME_SEPARATOR = TEXT("_");
const FString WARNING_HEADER = TEXT("WARNING :: [Interactive Snow Component] ::");


UInteractiveSnowComponent::UInteractiveSnowComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractiveSnowComponent::DrawMaterial(FVector2D UVs, UTexture2D* ShapeTexture, FVector2D TextureScale, bool bIsMainPlayer)
{
	// General setup and checks

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

	FVector2D drawMaterialScale = TextureScale;

	// Infinite / non-infinite specific setup
	/// Non-infinite just uses the regular 0-1 UV space
	/// Infinite only applies the displacement to a specific area around the player / interactor object

	if (!bInfiniteSurface)
	{
		DrawMaterialInstance->SetVectorParameterValue(LOCATION_PARAMETER_NAME, FLinearColor(UVs.X, UVs.Y, 0.f, 1.f));
	}
	else
	{
		// Prevent double scaling of draw material by applying the inverse scale of the displacement texture

		drawMaterialScale *= 1.f / DisplacementTextureScale;

		if (bIsMainPlayer)
		{
			// Scale render target texture to an area around the object/player (already calculated during BeginPlay).

			DynamicMaterial->SetScalarParameterValue(SCALE_X_PARAMETER_NAME, DisplacementTextureScale);
			DynamicMaterial->SetScalarParameterValue(SCALE_Y_PARAMETER_NAME, DisplacementTextureScale);

			// For infinite we move the cached texture instead of the object/player.
			/// Need to move texture in discrete steps to prevent blurring, by making it match with the texture pixels.

			FVector2D discreteUVs = GetPixelPerfectUvLocation(UVs, UvPixelSize);
			FVector2D distMoved = (discreteUVs - PrevUvLocation) * (1.f / DisplacementTextureScale); // The smaller the area, the more we have to offset to match real size area

			DynamicMaterial->SetVectorParameterValue(LOCATION_PARAMETER_NAME, FLinearColor(discreteUVs.X, discreteUVs.Y, 0.f, 1.f));
			DrawMaterialInstance->SetVectorParameterValue(LOCATION_PARAMETER_NAME, FLinearColor(0.5f, 0.5f, 0.f, 1.f)); // Always in the center since we are moving cached texture instead
			DrawMaterialInstance->SetScalarParameterValue(PREV_OFFSET_X_PARAMETER_NAME, distMoved.X);
			DrawMaterialInstance->SetScalarParameterValue(PREV_OFFSET_Y_PARAMETER_NAME, distMoved.Y);

			PrevUvLocation = discreteUVs;
		}
		else
		{
			// Calculate UV distance from main player/object and position shape there (taking displacement map scale into account as well)

			FVector2D discreteUVs = GetPixelPerfectUvLocation(UVs, UvPixelSize);
			FVector2D uvLocation = (discreteUVs - PrevUvLocation) * (1.f / DisplacementTextureScale);
			uvLocation = FVector2D(0.5f, 0.5f) + uvLocation; // Main object is always in the middle of the displacement map, so we need to get location from there

			DrawMaterialInstance->SetVectorParameterValue(LOCATION_PARAMETER_NAME, FLinearColor(uvLocation.X, uvLocation.Y, 0.f, 1.f));

			// Force these to 0 always to prevent moving the texture when the main object is not moving
			DrawMaterialInstance->SetScalarParameterValue(PREV_OFFSET_X_PARAMETER_NAME, 0.f);
			DrawMaterialInstance->SetScalarParameterValue(PREV_OFFSET_Y_PARAMETER_NAME, 0.f);
		}
	}

	// Apply common parameters

	DrawMaterialInstance->SetScalarParameterValue(SCALE_X_PARAMETER_NAME, drawMaterialScale.X);
	DrawMaterialInstance->SetScalarParameterValue(SCALE_Y_PARAMETER_NAME, drawMaterialScale.Y);

	// Draw on render targets

	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), RenderTarget, DrawMaterialInstance); // Actual render target
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), PrevRenderTarget, TextureCopyMaterialInstance); // Cached render target
}

void UInteractiveSnowComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = GetOwner(); // Need to delay this until BeginPlay so that it works when inherited by blueprints

	RenderTarget = CreateRenderTarget(RenderTargetResolution, ETextureRenderTargetFormat::RTF_R16f);
	PrevRenderTarget = CreateRenderTarget(RenderTargetResolution, ETextureRenderTargetFormat::RTF_R16f);

	UvPixelSize = 1.f / RenderTargetResolution;

	InitMaterials();

	if (bInfiniteSurface)
	{
		DisplacementTextureScale = GetDisplacementTextureScale(InfiniteSurfaceRenderArea, bInfiniteSurface);
	}
}

UTextureRenderTarget2D* UInteractiveSnowComponent::CreateRenderTarget(int32 Resolution, ETextureRenderTargetFormat Format)
{
	UTextureRenderTarget2D* newRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, Resolution, Resolution, Format);
	newRenderTarget->AddressX = TextureAddress::TA_Clamp;
	newRenderTarget->AddressY = TextureAddress::TA_Clamp;
	newRenderTarget->bAutoGenerateMips = false;

	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), newRenderTarget);

	return newRenderTarget;
}

float UInteractiveSnowComponent::GetDisplacementTextureScale(float RenderSize, bool bIsInfiniteRenderSurface) const
{
	if (!bIsInfiniteRenderSurface)
	{
		return 1.f;
	}

	FVector extents = StaticMeshComponent->GetStaticMesh()->GetBoundingBox().GetExtent();
	float largestSize = FMath::Max(extents.X, extents.Y) * 2.f;

	// We assume that UVs 0-1 space covers the largest axis.
	return FMath::Min(RenderSize / largestSize, 1.f); // Can't go over 0-1 space
}

FVector2D UInteractiveSnowComponent::GetPixelPerfectUvLocation(FVector2D UVs, float PixelSize) const
{
	return FVector2D((FMath::FloorToFloat(UVs.X / PixelSize) + 0.5f) * PixelSize, (FMath::FloorToFloat(UVs.Y / PixelSize) + 0.5f) * PixelSize);
}

void UInteractiveSnowComponent::InitMaterials()
{
	StaticMeshComponent = Cast<UStaticMeshComponent>(OwnerActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));

	// Exit early if actor doesn't have a static mesh component 

	if (!StaticMeshComponent)
	{
		LogWarning("No mesh component found on actor: " + OwnerActor->GetActorLabel() + ". Unable to initialize material.");
		return;
	}

	// Use object's own material as base material if it is null

	if (!BaseMaterial)
	{
		BaseMaterial = StaticMeshComponent->GetMaterial(0);
	}

	// Create dynamic material and assign it to the static mesh component

	FString materialName = OwnerActor->GetName() + NAME_SEPARATOR + BaseMaterial->GetName();
	DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this, FName(*materialName));
	DynamicMaterial->SetTextureParameterValue(RENDER_TARGET_PARAMETER_NAME, RenderTarget);

	StaticMeshComponent->SetMaterial(0, DynamicMaterial);

	// Create material for drawing on the render target as well

	if (!RenderTargetDrawMaterial)
	{
		LogWarning("Render target draw material is null. Unable to create dynamic material instance.");
		return;
	}

	FString renderTargetMaterialName = OwnerActor->GetName() + NAME_SEPARATOR + RenderTargetDrawMaterial->GetName();
	DrawMaterialInstance = UMaterialInstanceDynamic::Create(RenderTargetDrawMaterial, this, FName(*renderTargetMaterialName));
	DrawMaterialInstance->SetTextureParameterValue("PreviousRenderTexture", PrevRenderTarget);
	DrawMaterialInstance->SetScalarParameterValue("UV Pixel Size", UvPixelSize);

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
