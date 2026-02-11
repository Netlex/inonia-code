# Unreal Team Agents (UTA)

`Unreal Team Agents` — UE5-плагин (целевая версия **UE 5.7**) для AI-ассистирования разработки в Unreal Editor.

## Зафиксированные требования

- UE: **5.7**
- ОС: **Windows 10/11**
- LLM provider по умолчанию: **DeepSeek** (OpenAI-compatible API)
- Полная поддержка Blueprint на целевой траектории: **чтение, запись, создание**
- Основной фокус: **генерация C++ кода и Blueprint-графов**

## Текущий статус реализации

Уже реализован рабочий MVP-каркас с первыми runtime-компонентами:

- Модульный плагин с разделением на `UTACore`, `UTAChat`, `UTAUI`, `UTABlueprint`, `UTAEditor`.
- DeepSeek provider (`FUTADeepSeekProvider`) c HTTP-вызовом `/chat/completions`.
- Базовый оркестратор чата (`FUTAChatOrchestrator`) и in-memory conversation store.
- Tool registry и первые инструменты:
    - `read_file`
    - `search_files`
    - `write_file` (только с `approved=true`)
- Editor tab `Unreal Team Agents` с виджетом `SUTAChatPanel` и отправкой сообщений в orchestrator.

## Модульная архитектура

1. **UTACore** (Runtime)

    - контракты ядра (`IUTAAgentOrchestrator`, `IUTALLMProvider`, `IUTATool`, `IUTAToolRegistry`),
    - provider settings,
    - базовые file tools.

2. **UTAChat** (Runtime)

    - orchestration chat-loop,
    - управление сессиями,
    - роутинг tool-calls.

3. **UTAUI** (Editor)

    - Slate UI,
    - панель чата,
    - action-log / approval UI (этап расширения).

4. **UTABlueprint** (Editor)

    - инструменты работы с Blueprint graph (реализация расширяется),
    - создание/изменение нод (roadmap),
    - сериализация графа для контекста агента (roadmap).

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
    - `list_blueprints` (next)
    - `create_blueprint_asset` (next)

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
