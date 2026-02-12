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
- Политика безопасности путей:
    - read/search только внутри проекта,
    - write только в `Source/`, `Plugins/`, `Content/`,
    - deny по чувствительным директориям.
- Editor tab `Unreal Team Agents` с виджетом `SUTAChatPanel` и отправкой сообщений в orchestrator.
- Blueprint tools v1 подключены в tool registry:
    - `list_blueprints`
    - `read_blueprint_metadata`
    - `create_blueprint_asset`

## Настройка DeepSeek

Поддерживаются два канала конфигурации (env перекрывает ini):

1. `DefaultGame.ini` / `Saved/Config/*`:

```ini
[UTA.DeepSeek]
ApiKey=
BaseUrl=https://api.deepseek.com
ChatEndpoint=/chat/completions
Model=deepseek-chat
TimeoutSeconds=60.0
```

2. Переменные окружения:

- `UTA_DEEPSEEK_API_KEY`
- `UTA_DEEPSEEK_BASE_URL`
- `UTA_DEEPSEEK_MODEL`

## MVP tool protocol

Текущий минимальный протокол ручного вызова инструмента в чате:

```text
/tool <tool_name> <json-args>
```

Примеры:

```text
/tool read_file {"path":"Source/MyModule/MyFile.cpp"}
/tool search_files {"query":"TODO", "rootDir":"Source"}
/tool write_file {"path":"Source/MyModule/NewFile.txt", "content":"hello", "approved":true}
/tool list_blueprints {"packagePath":"/Game"}
/tool read_blueprint_metadata {"assetPath":"/Game/MyBP.MyBP"}
/tool create_blueprint_asset {"packagePath":"/Game/UTA", "assetName":"BP_Enemy", "parentClass":"Actor"}
```

## Модульная архитектура

1. **UTACore** (Runtime)

    - контракты ядра (`IUTAAgentOrchestrator`, `IUTALLMProvider`, `IUTATool`, `IUTAToolRegistry`),
    - provider settings,
    - execution policy,
    - базовые file tools.

2. **UTAChat** (Runtime)

    - orchestration chat-loop,
    - управление сессиями,
    - роутинг tool-calls,
    - загрузка provider-конфига.

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
