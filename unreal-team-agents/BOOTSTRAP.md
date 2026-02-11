# Bootstrap нового репозитория UTA

Целевой репозиторий GitHub:

- `https://github.com/Netlex/Unreal-Team-Agents`

## Быстрый вариант (автопубликация)

Из этой папки можно запустить скрипт:

```bash
./scripts/publish-to-netlex.sh
```

Что делает скрипт:

1. Собирает временную standalone-копию плагина `unreal-team-agents/`.
2. Инициализирует git-репозиторий.
3. Создаёт первый commit.
4. Пушит в `git@github.com:Netlex/Unreal-Team-Agents.git` (ветка `main`).

> Требуется рабочая SSH-авторизация к GitHub (ключ с доступом к репозиторию).

## Ручной вариант

```bash
mkdir unreal-team-agents-repo
cd unreal-team-agents-repo
git init
cp -R /path/to/unreal-team-agents/* .
git add .
git commit -m "chore: bootstrap Unreal Team Agents plugin scaffold"
git branch -M main
git remote add origin git@github.com:Netlex/Unreal-Team-Agents.git
git push -u origin main
```

## Папка для UE проекта

После публикации плагин должен находиться в:

```text
<YourUEProject>/Plugins/UnrealTeamAgents/
```

где `UnrealTeamAgents.uplugin` расположен в корне этой папки.
