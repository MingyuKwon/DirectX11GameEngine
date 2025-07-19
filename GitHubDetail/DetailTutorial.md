---

`Detail Window`에서는 현재 선택한 액터에 대한 자세한 정보를 보고, 수정할 수 있습니다

![Main Banner1](DetailMain1.png)


1. Actor의 이름을 보여줍니다. 만약 액터가 invisible이라면 회색으로 이름이 표시 됩니다


2. Actor의 Transform을 순서대로 x y z 순으로 보여줍니다. 회전은 오일러 각 기준입ㄴ디ㅏ
각 창에 숫자를 직접 입력하거나, 값을 드래그 해서 바꾸는 것이 가능합니다

![Explain GIF 2](Transform_Move.gif)
- 위치 부분

![Explain GIF 3](Transform_Rotate.gif)
- 회전 부분 (오일러 각으로 각 축을 기준으로 회전한다)

![Explain GIF 4](Transform_Scale.gif)
- 스케일 부분


3. Actor의 Static Mesh Component 관련해서 값을 바꿀 수 있습니다


- Enable Static Mesh 
![Explain GIF 5](StaticEnable.gif)
: static Mesh를 사용할 것인지 선택하는 토글입니다

- Mesh Merge
![Explain GIF 6](MeshMerge.gif)

: 액터의 Mesh를 Merge해서 보여줄 것인지 정합니다. 만약 Actor의 각 Mesh가 따로 따로 그려질 필요가 있다면 켜서는 안되지만. 각 메시별로 따로 따로 이동하는게 아니라면, 메시를 합쳐서 그리는 것으로 최적화를 자동 진행합니다

- Set Mesh
![Explain GIF 7](SetMesh.gif)

: 다른 메시로 메시를 고체합니다. 만약 메시에 텍스처 매핑 정보가 내장되어 있다면 자동으로 텍스처도 불러 와 집니다

- Set Texture
![Explain GIF 8](TextureChange.gif)

: 텍스처를 다른 텍스처로 교체합니다



![Main Banner2](DetailMain2.png)

4. Actor의 Light Component 관련해서 값을 바꿀 수 있습니다

![Explain GIF 6](LightComp.gif)
