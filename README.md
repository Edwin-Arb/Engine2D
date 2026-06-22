# Engine2D

A small **educational 2D engine in C++20** built on top of [SFML](https://www.sfml-dev.org/) 3.0.2.
It exists to learn two things hands-on: the **architecture** of a game engine (an Unreal-style
component model — `UWorld`, `AActor`, `USceneComponent`, render / physics / collision components)
and the **math behind real-time graphics and physics** (vectors, matrices, transforms, numerical
integration), shown through a set of interactive demos.

> 🌍 **Languages:** [English](#english) · [Русский](#русский)

---

## English

### Overview

Engine2D is a single Windows application that hosts a stack of **interactive math demos**. You
switch between them with the arrow keys; each demo isolates one concept (a vector operation, a
force law, a transform) and draws it on screen so it can be *seen and poked*, not just computed.

The long-term goal of the author is **render programming** (OpenGL → Vulkan), so the engine is
deliberately a sandbox for the math that underpins rendering rather than a finished product.

### Tech stack

| | |
|---|---|
| Language | C++20 (`/std:c++20`) |
| Library | SFML 3.0.2 (Graphics / Window / System / Audio), x64 |
| Build | Visual Studio 2022 (toolset **v143**) — also opens in JetBrains Rider |
| Platform | Windows, **x64** |

### Architecture

- **`UWorld`** — owns the lifetime of every `AActor` and the flat render registry. Actors are
  created through `SpawnActor<T>()` and torn down with `DestroyActor()`; it also drives the
  `RegisterAllActors → BeginPlay → Tick` lifecycle.
- **`AActor`** — an identity object (non-copyable, non-movable) that owns components. It is
  referenced by raw back-pointers, so it never relocates in memory.
- **`UActorComponent` / `USceneComponent`** — base components; scene components carry a 2D
  transform and form an attachment hierarchy (`AttachToComponent`).
- **`URenderComponent`** (→ `USquareRenderComponent`, `UCircleRenderComponent`) — self-register
  into the world's draw list via `OnRegister` / `OnUnregister`.
- **`UPhysicsComponent`** — semi-implicit (symplectic) Euler integrator: `velocity += a·dt`,
  then `position += velocity·dt`; force accumulation via `AddForce`.
- **`UCollisionComponent`** (→ `UCircleCollisionComponent`) — overlap tests and collision normals.
- **Fixed timestep** — the main loop (`Main.cpp`) advances the simulation in constant `FIXED_DT`
  steps via an accumulator, which keeps physics stable and deterministic.
- **`FDebugDraw`** — immediate-mode helper that batches lines / arrows / circles and flushes them
  in a single draw call (used by the visualization demos).
- **Math** — `FVector2D`, `FMatrix3x3`, `FTransform2D`, `FMath` (random / angle helpers).

### Demos

Every demo implements the `IMathDemo` interface (`Enter / Tick / Render / Exit`). Cycle through
them with the **Left / Right arrow keys**.

| Demo (HUD title) | Concept it demonstrates |
|---|---|
| **No Active Math Demo** | Start screen / scene playground |
| **Dot Product — Projection of B onto A** | Dot product as projection ("shadow"), its sign, and `A·B` shown live |
| **Cross Product — Left / Right Orientation** | 2D cross product sign = which side the target is on |
| **Steering — Smooth Rotation Towards Target** | Smoothly turning to face a target (`atan2(cross, dot)`) |
| **Rotation Inertia — Angular Velocity & Damping** | Angular momentum: overshoot + frame-rate-independent damping |
| **Field Of View — Vision Cone Detection** | Vision cone via a dot-product threshold |
| **Reflection — Raycast & Surface Reflection** | Ray–circle intersection and reflection `R = I − 2(I·N)N` |
| **Transform System — Local & World Space Hierarchy** | A chain of attached actors; matrix cascading (local → world) |
| **Camera Transform** | View-matrix manipulation: pan, rotate, zoom |
| **Gravity — Newtonian N-Body** | `F = G·m₁·m₂/r²`, circular orbits, force **superposition**, the Hill sphere |
| **Spring — Hooke's Law** | `F = −k·x`, damping toward critical, grab the cargo with the mouse |
| **Boids — Flocking** | Emergent flocking from cohesion + alignment + separation |
| **Lines — Vector Addition** | Vector add (parallelogram) and the perpendicular, fully draggable |
| **Matrix — Rotation & Translation** | A matrix as **basis vectors**: drag the X/Y axes and watch a square transform |

### Controls

**Global**
- **← / →** — switch to the previous / next demo
- **W A S D** — thrust the player square
- **Hold Space** — autopilot the square toward the target circle
- **Hold Left Mouse** — autopilot the square toward the cursor
- **R** — instantly turn the square to face the target

**Per-demo (mouse)**
- **Spring / Lines / Dot Product / Matrix** — drag the colored handles (points / vector tips / basis axes) with the **Left Mouse Button**
- **Camera Transform** — `WASD` move camera, `LMB / RMB` rotate, mouse wheel zoom, `Space` reset

### Build & Run

1. **Get SFML 3.0.2** (Visual C++ / x64) from <https://www.sfml-dev.org/download/> — it is **not**
   stored in the repo (`SFML/` is git-ignored). Unpack it next to the solution so you get:
   ```
   SFML/
     include/
     lib/
     bin/
   ```
   The project paths already point here (`$(SolutionDir)SFML/include`, `$(SolutionDir)SFML/lib`),
   and a post-build step copies the required `.dll`s next to the built `.exe` automatically.
2. Open **`Engine2D.sln`** in Visual Studio 2022 (or Rider).
3. Select the **`Debug | x64`** (or `Release | x64`) configuration.
4. **Build & Run (F5).** Running from the IDE sets the working directory to the project folder, so
   the font at `Engine2D/Font/PFAgoraSlabPro Bold.ttf` (loaded at startup) is found. Launching the
   raw `.exe` from `x64/Debug` won't find the font unless the working directory is the project folder.

### Project structure

```
Engine2D/
  Engine2D/
    Source/
      Core/          GameMain, UWorld, FDemoContext, Constants, Logging, Main
      Components/
        Base/        UActorComponent, USceneComponent, URenderComponent, UPhysicsComponent
        Render/      USquareRenderComponent, UCircleRenderComponent
        Collisions/  UCollisionComponent, UCircleCollisionComponent
        AActor
      Math/          FVector2D, FMatrix3x3, FTransform2D, FMath, FCamera
      Demos/         IMathDemo + one file per demo, FDebugDraw lives in Core/
    Font/            runtime font(s)
    Engine2D.vcxproj
  SFML/              third-party, not committed (see above)
  Engine2D.sln
  .clang-format      code style (kept identical at root and in Engine2D/)
  README.md
```

### Code style

Formatting is defined by `.clang-format` (Microsoft base, **Allman braces, real tabs**,
`PointerAlignment: Left`, wide column limit so math expressions are not split). Naming follows
Unreal-style conventions: `F`/`U`/`A` type prefixes, `PascalCase` members, `In`/`Out`-prefixed
parameters, `b` for booleans.

### Roadmap

The math / visualization phase is complete. Next steps follow the render-programming path:
**OpenGL** (window → triangle → shaders → textures → transforms → camera → lighting) → **Vulkan** →
rendering techniques (materials, optimization). The vectors, dot products, normals and matrices
built here map directly onto that work.

---

## Русский

### О проекте

Engine2D — это одно Windows-приложение, в котором живёт набор **интерактивных
математических демок**. Между ними переключаются стрелками; каждая демка изолирует один концепт
(операцию над векторами, закон силы, преобразование) и рисует его на экране, чтобы его можно было
*увидеть и потрогать*, а не только посчитать.

Долгосрочная цель автора — **render-программирование** (OpenGL → Vulkan), поэтому движок намеренно
сделан песочницей для математики, лежащей в основе рендеринга, а не законченным продуктом.

### Технологии

| | |
|---|---|
| Язык | C++20 (`/std:c++20`) |
| Библиотека | SFML 3.0.2 (Graphics / Window / System / Audio), x64 |
| Сборка | Visual Studio 2022 (toolset **v143**) — также открывается в JetBrains Rider |
| Платформа | Windows, **x64** |

### Архитектура

- **`UWorld`** — владеет временем жизни всех `AActor` и плоским реестром рендера. Акторы создаются
  через `SpawnActor<T>()` и удаляются через `DestroyActor()`; он же гоняет жизненный цикл
  `RegisterAllActors → BeginPlay → Tick`.
- **`AActor`** — identity-объект (не копируется, не перемещается), владеет компонентами. На него
  ссылаются сырыми указателями, поэтому он не двигается в памяти.
- **`UActorComponent` / `USceneComponent`** — базовые компоненты; сценовые несут 2D-трансформ и
  образуют иерархию привязки (`AttachToComponent`).
- **`URenderComponent`** (→ `USquareRenderComponent`, `UCircleRenderComponent`) — сами
  регистрируются в списке отрисовки мира через `OnRegister` / `OnUnregister`.
- **`UPhysicsComponent`** — полу-неявный (симплектический) интегратор Эйлера: `velocity += a·dt`,
  затем `position += velocity·dt`; накопление силы через `AddForce`.
- **`UCollisionComponent`** (→ `UCircleCollisionComponent`) — проверки пересечений и нормали столкновений.
- **Фиксированный шаг времени** — главный цикл (`Main.cpp`) двигает симуляцию шагами постоянного
  `FIXED_DT` через аккумулятор; это держит физику стабильной и детерминированной.
- **`FDebugDraw`** — immediate-mode хелпер: копит линии / стрелки / окружности и выгружает их одним
  вызовом отрисовки (его используют демки визуализации).
- **Математика** — `FVector2D`, `FMatrix3x3`, `FTransform2D`, `FMath` (random / угловые хелперы).

### Демки

Каждая демка реализует интерфейс `IMathDemo` (`Enter / Tick / Render / Exit`). Переключение —
**стрелками влево / вправо**.

| Демка (заголовок в HUD) | Что показывает |
|---|---|
| **No Active Math Demo** | Стартовый экран / песочница сцены |
| **Dot Product — Projection of B onto A** | Скалярное произведение как проекция («тень»), его знак и живое `A·B` |
| **Cross Product — Left / Right Orientation** | Знак 2D-векторного произведения = с какой стороны цель |
| **Steering — Smooth Rotation Towards Target** | Плавный доворот к цели (`atan2(cross, dot)`) |
| **Rotation Inertia — Angular Velocity & Damping** | Угловой импульс: перелёт + кадронезависимое затухание |
| **Field Of View — Vision Cone Detection** | Конус зрения через порог скалярного произведения |
| **Reflection — Raycast & Surface Reflection** | Пересечение луча с кругом и отражение `R = I − 2(I·N)N` |
| **Transform System — Local & World Space Hierarchy** | Цепочка привязанных акторов; каскад матриц (локаль → мир) |
| **Camera Transform** | Манипуляции матрицей вида: панорама, поворот, зум |
| **Gravity — Newtonian N-Body** | `F = G·m₁·m₂/r²`, круговые орбиты, **суперпозиция** сил, сфера Хилла |
| **Spring — Hooke's Law** | `F = −k·x`, затухание к критическому, захват груза мышью |
| **Boids — Flocking** | Эмерджентная стая: cohesion + alignment + separation |
| **Lines — Vector Addition** | Сложение векторов (параллелограмм) и перпендикуляр, всё перетаскиваемо |
| **Matrix — Rotation & Translation** | Матрица как **базисные векторы**: тащишь оси X/Y — квадрат преобразуется |

### Управление

**Глобально**
- **← / →** — предыдущая / следующая демка
- **W A S D** — тяга квадрата-игрока
- **Удерживать Space** — автопилот квадрата к кругу-цели
- **Удерживать ЛКМ** — автопилот квадрата к курсору
- **R** — мгновенно развернуть квадрат к цели

**Внутри демок (мышь)**
- **Spring / Lines / Dot Product / Matrix** — таскай цветные ручки (точки / концы векторов / оси базиса) **левой кнопкой мыши**
- **Camera Transform** — `WASD` двигают камеру, `ЛКМ / ПКМ` поворот, колесо — зум, `Space` — сброс

### Сборка и запуск

1. **Скачай SFML 3.0.2** (Visual C++ / x64) с <https://www.sfml-dev.org/download/> — в репозитории её
   **нет** (`SFML/` в `.gitignore`). Распакуй рядом с решением, чтобы получилось:
   ```
   SFML/
     include/
     lib/
     bin/
   ```
   Пути в проекте уже настроены сюда (`$(SolutionDir)SFML/include`, `$(SolutionDir)SFML/lib`), а
   пост-сборочный шаг автоматически копирует нужные `.dll` рядом с собранным `.exe`.
2. Открой **`Engine2D.sln`** в Visual Studio 2022 (или Rider).
3. Выбери конфигурацию **`Debug | x64`** (или `Release | x64`).
4. **Собери и запусти (F5).** Запуск из IDE ставит рабочую папку в каталог проекта, поэтому шрифт
   `Engine2D/Font/PFAgoraSlabPro Bold.ttf` (грузится на старте) находится. Если запускать `.exe`
   напрямую из `x64/Debug`, шрифт не найдётся, пока рабочая папка не указывает на каталог проекта.

### Структура проекта

```
Engine2D/
  Engine2D/
    Source/
      Core/          GameMain, UWorld, FDemoContext, Constants, Logging, Main
      Components/
        Base/        UActorComponent, USceneComponent, URenderComponent, UPhysicsComponent
        Render/      USquareRenderComponent, UCircleRenderComponent
        Collisions/  UCollisionComponent, UCircleCollisionComponent
        AActor
      Math/          FVector2D, FMatrix3x3, FTransform2D, FMath, FCamera
      Demos/         IMathDemo + по файлу на демку, FDebugDraw лежит в Core/
    Font/            рантайм-шрифт(ы)
    Engine2D.vcxproj
  SFML/              сторонняя, не коммитится (см. выше)
  Engine2D.sln
  .clang-format      стиль кода (одинаков в корне и в Engine2D/)
  README.md
```

### Стиль кода

Форматирование задаётся `.clang-format` (база Microsoft, **Allman-скобки, настоящие табы**,
`PointerAlignment: Left`, широкий лимит строки, чтобы математику не разрывало). Нейминг — в стиле
Unreal: префиксы типов `F`/`U`/`A`, `PascalCase` для членов, параметры с префиксами `In`/`Out`,
`b` для булевых.

### Дорожная карта

Фаза математики / визуализации завершена. Дальше — по пути render-программиста: **OpenGL** (окно →
треугольник → шейдеры → текстуры → трансформы → камера → освещение) → **Vulkan** → техники рендера
(материалы, оптимизация). Вектора, скалярные произведения, нормали и матрицы, собранные здесь,
ложатся прямо на эту работу.
