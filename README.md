## 日本語は下記です。

# Overview
![Main Image](https://github.com/Zhibade/interactive-snow-unreal/raw/master/Docs/Main.jpg)
![Interactive Snow On Objects](https://github.com/Zhibade/interactive-snow-unreal/raw/master/Docs/InteractiveSnowOnObjects.gif)
![Object Snow Shader](https://github.com/Zhibade/interactive-snow-unreal/raw/master/Docs/ObjectSnowShader.gif)
![Custom Hole Shapes](https://github.com/Zhibade/interactive-snow-unreal/raw/master/Docs/HoleShapes.jpg)

Unreal interactive snow implementation. C++ and shaders.

### Features
- Interactive snow on any object (requires specific setup on UVs)
- "Infinite" surfaces support
- Object-agnostic snow shader
- Custom hole shapes

**Disclaimer**: This project was made for learning/experimentation/portfolio purposes. It is not meant to be a production-ready solution, but rather a good starting point. There are still many edge cases and features that could be added. If you have any questions please let me know.


# How to use
- Open the project
- Press *Play*
- Move the sphere around with WASD


# Implementation Outline
In essence, it draws the hole shapes to a render target that determines where the snow should be removed. Where and how it draws is determined by the collision of the *interactor* object with the *interactive snow* surface. On infinite surfaces, instead of using the render target on the entire object it only keeps track of the area around the *main player*.

The object snow shader displaces the vertices and transitions to a world-UV snow material based on the Z axis of the object. Snow objects can also have interactive snow enabled but they have to follow specific guidelines in the UV setup for it to work properly.

Below is a general overview of the C++ and Shader implementations. For more details please take a look at the code and Unreal files directly.

## C++

- **InteractiveSnowComponent:** This component adds the snow functionality to a surface. Any *SnowInteractorComponent* that collides with this surface will create holes on it.

- **SnowInteractorComponent:** This component makes any object be able to interact with an *InteractiveSnowComponent*. The hole shape and size are controlled by its parameters.

- **SpherePawn:** Minimalistic pawn for moving the sphere around with physics.

- **InteractiveSnowSurface:** Wrapper class for an actor with a *StaticMeshComponent* and an *InteractiveSnowComponent*. Not really required.

## Material Functions

All the snow features are handled by material functions and not the materials themselves.

- **MF_Displacement:** Handles the displacement and tessellation of the snow. White = hole, black = raised snow.

- **MF_CustomUvTransform:** Handles all the UV transformations of the hole shape texture to place it correctly in the snow displacement map.

- **MF_ObjectSnow:** Handles all snow material masking and blending for objects with snow on top.

- **MF_CleanBorder:** Adds a 1px black border around a texture to avoid texture clamping issues.

- **MF_Step:** Regular [Step function](https://developer.download.nvidia.com/cg/step.html) implementation (Unreal apparently doesn't have this built-in).


# Requirements for new objects:
- Any object that will act as a snow surface (have snow that can be removed), has the following UV requirements:
  - 0-1 UVs in UV0, UV1 or UV2.
  - As little distortion per-polygon as possible (non-uniform scaling is supported)
  - No seams around the snow area
  - When the *interactor* hole shape is not symmetrical, the *interactive* object UVs have to **match the world orientation** of Unreal. Otherwise the hole rotation will be off.

- Hole shapes are controlled by textures. If the shapes are not symmetrical, the **top** of the image is considered the forward axis of the object (matching with X+ in Unreal).

- Because the object snow shader uses displacement, objects that use it require **soft edges** on the affected area. This is because hard edges will create holes in the mesh when displaced.



# 概要
UE4でインタラクティブ雪実装。C++コードとシェーダ。

### 機能
- オブジェクト形に関わらず、インタラクティブ雪が可能です。（UVの特定設定の必要があります）
- 無限表面可能
- オブジェクト形に関わらず雪シェーダ
- カスタム穴形可能

#### 注意:
これは個人的なプロジェクトです。目的は習ったり、実験したり、ポートフォリオ作りたりすることです。そのままリリース準備完了ではありませんが、いっそこのプロジェクトは良い出発点だと思います。ご質問があればご連絡してください。


# 使い方
- UE4でプロジェクトを開いて。
- 「Play」ボタンをクリックして。
- 「WASD」でボール操作できます。


# 実装要旨
基本的に、穴テクスチャを描画ターゲットに描きます。描画ターゲットに基づいて雪の高さとマスクを決めます。「Interactor」オブジェクトから「InteractiveSnow」オブジェクトにコリジョンによって、穴テクスチャの位置と回転とサイズを変更します。無限表面の場合では、描画ターゲットは全部のオブジェクトの面積に合わせの代わりに、プレイヤーの特定周りだけに描画ターゲットを合わせます。

オブジェクトの雪のシェーダについては、Z軸によって、頂点を移動し、テクスチャのマスクを変遷します。この雪はインタラクティブになることが可能ですが、UVの特定の条件に合わせることが必要です。

C++とシェーダの概要は下記です。もし、もっと細かく詳細欲しかったら、直接にコードとUE4のアセットをご確認ください。

## C++

- **InteractiveSnowComponent:** インタラクティブ雪の機能を有効します。全ての「SnowInteractorComponent」はこのオブジェクトに穴を描きます。

- **SnowInteractorComponent:** 「InteractiveSnoｗComponent」を対応します。穴の形とサイズはパラメータによって、変化します。

- **SpherePawn:** 物理的なボール操作のためのポーンです。

- **InteractiveSnowSurface:** アクタのラッパーのクラスです。「StaticMeshComponent」と「InteractiveSnoｗComponent」が含まれています。必要がありません。

## マテリアル関数

全ての機能はマテリアルの代わりに、マテリアル関数に含まれています。

- **MF_Displacement:** ディスプレイスメントとテッセレーション管理。白：穴、黒：雪。

- **MF_CustomUvTransform:** 穴のテクスチャのUVトランスフォーム管理。

- **MF_ObjectSnow:** オブジェクトの雪のマスクと変遷管理。

- **MF_CleanBorder:** テクスチャの境界クランプの問題防止のために、テクスチャの境界に黒い１ピクセル追加。

- **MF_Step:** 普通[Step関数](https://developer.download.nvidia.com/cg/step.html)の実装。UE4にこの関数がなさそうです。


# 新しいオブジェクトの条件
- 正しいオブジェクトの雪の機能の有効のために、下記のUV条件が必要です。
  - UV0、UV1、またはUV2に０～１座標内の必要があります。
  - 各ポリゴンはできるだけ少なく歪曲に展開。不均等スケーリング対応できます。
  - 雪の周りに、UVのシームが対応できません。
  - 不対称的な穴のテクスチャの場合は、UVの回転はワールドの同じの回転が必要です。回転は間違っているの時に、穴の回転が正しくなくなってしまいます。

- 穴の形はテクスチャに基づいています。不対称の場合は、テクスチャの上がオブジェクトの正面軸になります（UE4のX+軸）、そして、「InteractiveSnowComponent」のUVはUE4のワールド軸に合わせることの必要があります。

- オブジェクトの雪のシェーダはディスプレイスメントを使用しているので、雪の部分にソフト頂点法線の必要があります。ハード頂点法線の場合には、ディスプレイスメントの時にメッシュが割れてしまいます。