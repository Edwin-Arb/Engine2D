# Engine2D

Учебный 2D-движок на C++ поверх [SFML](https://www.sfml-dev.org/) 3.0.2.
Цель проекта — разобраться в архитектуре игрового движка (компонентная модель в духе
Unreal: `AActor` + `UActorComponent`, сцена, рендер, коллизии) и в математике, лежащей
в основе рендеринга.

## Структура

```
Engine2D/
  Source/
    Core/          GameMain, контекст демо, константы
    Components/    AActor и компоненты (Base / Render / Collisions)
    Math/          математика (трансформации, векторы)
    Demos/         демо-сцены
  Font/            шрифты, используемые в рантайме
Engine2D.sln       решение Visual Studio
```

## Зависимости

Проект использует **SFML 3.0.2** (x64). Сама библиотека **не хранится в репозитории**
(папка `SFML/` в `.gitignore`), поэтому после клонирования её нужно положить рядом.

### Как подключить SFML

1. Скачать SFML 3.0.2 для Visual Studio (x64) с https://www.sfml-dev.org/download/
2. Распаковать в папку `SFML/` в корне проекта так, чтобы получилось:
   ```
   SFML/
     include/
     lib/
     bin/
   ```
3. Пути в проекте `Engine2D.vcxproj` уже настроены на эту папку
   (`$(SolutionDir)SFML/include`, `$(SolutionDir)SFML/lib`).
4. `.dll` из `SFML/bin/` должны лежать рядом с собранным `.exe`
   (или в `PATH`) — иначе приложение не запустится.

## Сборка

1. Открыть `Engine2D.sln` в Visual Studio или JetBrains Rider.
2. Конфигурация: `Debug` / `x64`.
3. Собрать и запустить.

## Форматирование

Стиль кода описан в `.clang-format`.
