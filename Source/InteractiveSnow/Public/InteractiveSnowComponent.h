// Originally made by Jose Ivan Lopez Romo (https://www.ivanlopezr.com)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "InteractiveSnowComponent.generated.h"


// This component enables the interaction with snow surfaces. It requires a static mesh component to be present on the actor.
// NOTE: Requires 0-1 UVs
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INTERACTIVESNOW_API UInteractiveSnowComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractiveSnowComponent(const FObjectInitializer& ObjectInitializer);

	/**
	* Draws the given shape in this surface using the UV location, texture, and texture scale
	*
	* @param UVs - UV location of the hole
	* @param ShapeTexture - Texture to use when drawing
	* @param TextureScale - Scale value to apply when drawing on the texture
	*/
	UFUNCTION(BlueprintCallable)
	void DrawMaterial(FVector2D UVs, UTexture2D* ShapeTexture, FVector2D TextureScale);

protected:
	UPROPERTY()
	AActor* OwnerActor = nullptr;

	UPROPERTY()
	UStaticMeshComponent* StaticMeshComponent = nullptr;

	UPROPERTY()
	UTexture2D* CurrentShapeTexture = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* DrawMaterialInstance = nullptr;

	UPROPERTY()
	UMaterialInstanceDynamic* TextureCopyMaterialInstance = nullptr;

	UPROPERTY()
	UTextureRenderTarget2D* PrevRenderTarget = nullptr;

	UPROPERTY()
	UTextureRenderTarget2D* RenderTarget = nullptr;


	// --- INFINITE SURFACE PROPERTIES --- //

	UPROPERTY()
	float DisplacementTextureScale = 1.f;

	UPROPERTY()
	float UvPixelSize = 1.f;

	UPROPERTY()
	FVector2D PrevUvLocation = FVector2D::ZeroVector; // Used for "infinite" surfaces only.


	// --- EXPOSED PROPERTIES --- //

	// Toggles optimization for large or "infinite" surfaces. NOTE: It is intended for plane-like surfaces mostly.
	UPROPERTY(EditAnywhere)
	bool bInfiniteSurface = false;

	// Active render area when using an "infinite" surface. Centimeters.
	UPROPERTY(EditAnywhere)
	float InfiniteSurfaceRenderArea = 2000.f;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* BaseMaterial = nullptr;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* RenderTargetCopyMaterial = nullptr;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* RenderTargetDrawMaterial = nullptr;

	UPROPERTY(EditAnywhere)
	int32 RenderTargetResolution = 1024;


	// --- FUNCTIONS / METHODS --- //

	virtual void BeginPlay() override;

	/**
	* Creates and initializes a new render target texture of the given resolution and format
	*
	* @param Resolution - Pixel resolution in X and Y
	* @param Format - Format to use for this render target
	*
	* @return New initialized render target
	*/
	UFUNCTION(BlueprintCallable)
	UTextureRenderTarget2D* CreateRenderTarget(int32 Resolution, ETextureRenderTargetFormat Format);

	/**
	* Returns the appropiate displacement texture scale according to the given parameters
	*
	* @param RenderSize - Render size of the displacement area in CM
	* @param bIsInfiniteSurface - Whether surface is infinite or not
	*
	* @return Uniform texture scale to apply to the displacement texture
	*/
	UFUNCTION(BlueprintCallable)
	float GetDisplacementTextureScale(float RenderSize, bool bIsInfiniteRenderSurface) const;

	/**
	* Initializes dynamic material instance for this actor, and the material for the render target
	*/
	UFUNCTION(BlueprintCallable)
	void InitMaterials();

	/**
	* Logs a warning using a preset header + the given message.
	*
	* @param Message - Message to log
	*/
	UFUNCTION(BlueprintCallable)
	void LogWarning(FString Message);
};
