# Unreal Team Agents (UTA)

`Unreal Team Agents` — новый UE5-плагин (целевой движок **UE 5.7**) для AI-ассистирования разработки в Unreal Editor.

## Зафиксированные требования

- UE: **5.7**
- ОС: **Windows 10/11**
- LLM provider по умолчанию: **DeepSeek** (OpenAI-compatible API)
- Полная поддержка Blueprint на целевой траектории: **чтение, запись, создание**
- Основной фокус: **генерация C++ кода и Blueprint-графов**

## Модульная архитектура

Плагин сразу разделён на отдельные модули, чтобы MVP можно было расширить до full-функционала без ломки API:

1. **UTACore** (Runtime)

    - контракты ядра (`IUTAAgentOrchestrator`, `IUTALLMProvider`, `IUTATool`),
    - execution policy,
    - единый формат tool results.

2. **UTAChat** (Runtime)

    - orchestration chat-loop,
    - управление сессиями,
    - роутинг tool-calls.

3. **UTAUI** (Editor)

    - Slate UI,
    - панель чата,
    - action-log / approval UI.

4. **UTABlueprint** (Editor)

    - инструменты работы с Blueprint graph,
    - создание/изменение нод,
    - сериализация графа для контекста агента.

5. **UTAEditor** (Editor, PostEngineInit)
    - интеграция с Editor lifecycle,
    - регистрация tab/menu/commands,
    - сборка модулей в единый runtime.

## MVP v1 (вертикальный срез)

- Dockable tab в UE Editor.
- Chat + DeepSeek completion.
- Инструменты:
    - `read_file`
    - `search_files`
    - `write_file` (только через explicit confirm)
- Базовый blueprint-path:
    - `list_blueprints`
    - `create_blueprint_asset`

## План расширения Blueprint (до full support)

### Stage B1

- read-only introspection графов (`get_blueprint_graph`, `get_blueprint_nodes`).

### Stage B2

- controlled writes (`add_node`, `connect_pins`, `set_default_values`) с preview/confirm.

### Stage B3

- full generation workflows:
    - генерация Blueprint целиком из спецификации,
    - рефакторинг графа,
    - repair broken links/compile errors.

## Структура

```text
unreal-team-agents/
├── UnrealTeamAgents.uplugin
└── Source/
    ├── UTACore/
    ├── UTAChat/
    ├── UTAUI/
    ├── UTABlueprint/
    └── UTAEditor/
```

## Следующий шаг

Далее можно добавить реализацию первых concrete классов:

- `FUTADeepSeekProvider` (в `UTACore`),
- `FUTAChatOrchestrator` (в `UTAChat`),
- `SUTAChatPanel` (в `UTAUI`),
- `FUTABlueprintToolset` (в `UTABlueprint`).
