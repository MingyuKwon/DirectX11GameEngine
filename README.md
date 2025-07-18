# 🎮 DirectX11GameEngine

**DirectX11**, **Win32 API**, **Dear ImGui**를 활용해 제작한 커스텀 게임 엔진입니다.

![Main Banner1](GitHubDetail/SampleTitle3.png)
![Main Banner2](GitHubDetail/SampleTitle2.png)
![Main Banner3](GitHubDetail/SampleTitle1.png)


---

## 📌 프로젝트 소개

이 프로젝트는 로우레벨부터 직접 구현한 게임 엔진으로, 다음과 같은 핵심 시스템을 포함합니다:

- 🔧 DirectX11 기반의 커스텀 렌더러
- 🧱 Win32 API를 이용한 윈도우 및 입력 처리
- 🖱️ 디버그 및 에디터용 UI 구현 (Dear ImGui 사용)
- 🧩 향후 확장 가능한 모듈형 구조 설계


## 🛠️ 사용 기술

- **그래픽스 API**: DirectX11  
- **플랫폼 레이어**: Win32 API  
- **UI 디버깅 도구**: Dear ImGui  


## 📆 개발 기간

- **렌더러부터 에디터까지 구현**  
  ⏰ *2025년 6월 21일 ~ 2025년 7월 15일*


## 📁 폴더 구조 소개

프로젝트는 다음과 같은 주요 폴더로 구성되어 있습니다:

- **🧠 KMGLib**  
  직접 구현한 게임 로직 및 엔진 코드가 위치한 폴더입니다.  
  렌더링, 입력 처리, 물리 처리 등 핵심 시스템을 담당합니다.

- **📦 OuterLib**  
  외부 라이브러리 및 참고 자료에서 가져온 코드가 들어 있는 폴더입니다.  
  예를 들어, Dear ImGui 소스, 블로그/문서 기반 구현 등이 포함됩니다.

- **🎨 Resource**  
  텍스처, 메시 등 렌더링에 필요한 리소스 파일이 들어 있는 폴더입니다.  
  씬에 배치할 오브젝트나 UI 요소 등의 시각 자료를 관리합니다.
