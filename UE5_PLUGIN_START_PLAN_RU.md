# Unreal Team Agents (UTA): стартовый план реализации

Этот документ обновлён под зафиксированные требования:

- UE **5.7**,
- Windows **10/11**,
- LLM: **DeepSeek**,
- фокус: генерация **C++ + Blueprint**,
- обязательная модульность: **core/editor/ui/chat** (+ blueprint).

## 1) С чего стартуем (практически)

Начинаем с MVP-вертикали, которая уже совместима с будущим full-функционалом:

1. `UTACore` — контракты и безопасность.
2. `UTAChat` — orchestration loop и DeepSeek provider integration.
3. `UTAUI` — chat tab + approval actions.
4. `UTABlueprint` — базовые blueprint tools (создание/чтение).
5. `UTAEditor` — регистрация tab/menu/команд.

## 2) Модульные границы

### UTACore

- Агентные контракты (`IUTAAgentOrchestrator`, `IUTALLMProvider`, `IUTATool`).
- Политики доступа (read/search auto-allow, write/create/edit confirm-required).
- Общая схема результата инструмента.

### UTAChat

- Управление диалогом и task-loop.
- Вызов инструментов через registry.
- Потоковое формирование ответа (стриминг токенов).

### UTAUI

- Dockable tab (Slate).
- History, prompt input, action timeline.
- Подтверждение опасных операций.

### UTABlueprint

- Инструменты чтения графов и структуры Blueprint.
- Создание Blueprint-ассетов по шаблонам.
- Контролируемая запись в граф (через preview + confirm).

### UTAEditor

- Жизненный цикл и registration layer.
- Интеграция с Editor subsystem/menus/commands.

## 3) MVP (2–3 недели)

### Sprint 1

- Каркас плагина + модули + `.uplugin`.
- DeepSeek provider (sync запрос + error handling).
- Chat tab с plain-text ответом.

### Sprint 2

- Tools: `read_file`, `search_files`, `write_file`.
- Approval policy и action log.
- Ограничение доступа workspace root + denylist.

### Sprint 3

- Blueprint v1: `list_blueprints`, `create_blueprint_asset`, `read_blueprint_metadata`.
- Генерация простого C++ класса (Actor/Component).

## 4) Blueprint full support roadmap (после MVP)

### Phase BP-A

- Полный read graph API:
    - `get_graphs`, `get_nodes`, `get_pins`, `get_connections`.

### Phase BP-B

- Write graph API:
    - `add_node`, `remove_node`, `connect`, `disconnect`, `set_node_property`.

### Phase BP-C

- End-to-end generation:
    - создание Blueprint из текстовой спецификации,
    - автосборка графа,
    - исправление compile/runtime blueprint проблем.

## 5) Стабильность и расширяемость (архитектурные правила)

1. **Строгое API-ядро**: UI/Editor не знают детали провайдера LLM.
2. **Tool registry + schema versioning**: новые инструменты добавляются без breaking changes.
3. **Event bus для телеметрии/логов**: чтобы легко подключать мониторинг.
4. **Policy engine** отдельно от UI: безопасность не должна зависеть от виджетов.
5. **Blueprint adapter слой**: изолирует API UE, упрощает миграции между версиями UE.

## 6) Что делаем прямо следующим шагом

1. Инициализируем новый репозиторий: `unreal-team-agents`.
2. Переносим текущий scaffold модулей в этот репозиторий.
3. Добавляем `FUTADeepSeekProvider`.
4. Добавляем `SUTAChatPanel` + отправка запроса в provider.
5. Реализуем `read_file`/`search_files` и approve flow для `write_file`.
