#!/usr/bin/env bash
set -euo pipefail

TARGET_URL="git@github.com:Netlex/Unreal-Team-Agents.git"
WORKDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TMP_DIR="$(mktemp -d)"

cleanup() {
  rm -rf "$TMP_DIR"
}
trap cleanup EXIT

echo "[UTA] Preparing temporary standalone repo in: $TMP_DIR"

rsync -a --exclude '.git' "$WORKDIR/" "$TMP_DIR/"

pushd "$TMP_DIR" >/dev/null
git init
git add .
git commit -m "chore: bootstrap Unreal Team Agents plugin scaffold"
git branch -M main
git remote add origin "$TARGET_URL"

echo "[UTA] Pushing to $TARGET_URL"
git push -u origin main --force
popd >/dev/null

echo "[UTA] Done. Repository published to $TARGET_URL"
