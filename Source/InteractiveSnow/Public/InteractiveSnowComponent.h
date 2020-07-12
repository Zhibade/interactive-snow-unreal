// Originally made by Jose Ivan Lopez Romo (https://www.ivanlopezr.com)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "InteractiveSnowComponent.generated.h"


// This component enables the interaction with snow surfaces. It requires a static mesh component to be present on the actor.
// NOTE: Requires 0-1 UVs in UV0, UV1 or UV2
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INTERACTIVESNOW_API UInteractiveSnowComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractiveSnowComponent(const FObjectInitializer& ObjectInitializer);

	/**
	* Draws the given shape in this surface using the UV location, texture, and texture scale.
	* When it is an infinite surface, it requires an additional parameter that indicates the main player/object.
	*
	* @param UVs - UV location of the hole
	* @param ShapeTexture - Texture to use when drawing
	* @param TextureScale - Scale value to apply when drawing on the texture
	* @param TextureRotation - Rotation value to apply when drawing on the texture (0-1 matches 0-360 rotation) (Assumes that the top part of the image is the X+ axis)
	* @param bIsMainPlayer - Indicates whether this is the main player/object or not (only relevant when it is set as infinite)
	*/
	UFUNCTION(BlueprintCallable)
	void DrawMaterial(FVector2D UVs, UTexture2D* ShapeTexture, FVector2D TextureScale, float TextureRotation, bool bIsMainPlayer = false);

	/**
	* Returns the used UV channel for this snow component
	*
	* @return Used UV channel as an int (UV0 = 0, UV1 = 1, etc)
	*/
	UFUNCTION(BlueprintCallable)
	int32 GetUsedUvChannel() const;

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

	// Base material to use for the surface/object
	UPROPERTY(EditAnywhere)
	UMaterialInterface* BaseMaterial = nullptr;

	// Material used when copying render targets. NOTE: Please don't change this unless you 100% understand what this is doing.
	UPROPERTY(EditAnywhere)
	UMaterialInterface* RenderTargetCopyMaterial = nullptr;

	// Material used when drawing to displacement render targets. NOTE: Please don't change this unless you 100% understand what this is doing.
	UPROPERTY(EditAnywhere)
	UMaterialInterface* RenderTargetDrawMaterial = nullptr;

	UPROPERTY(EditAnywhere)
	int32 RenderTargetResolution = 1024;

	// UV channel to use for the snow displacement
	UPROPERTY(EditAnywhere)
	int32 UvChannel = 0;


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
	* Takes an UV location and makes it pixel perfect.
	*
	* @param UVs - UV location to make pixel perfect
	* @param PixelSize - Pixel size that correspond to the given UVs
	*
	* @return Same UV location but pixel perfect (matching texture pixels)
	*/
	UFUNCTION(BlueprintCallable)
	FVector2D GetPixelPerfectUvLocation(FVector2D UVs, float PixelSize) const;

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
