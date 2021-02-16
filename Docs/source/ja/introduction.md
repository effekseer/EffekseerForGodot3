# 概要

## はじめに
これはEffekseerForGodotのドキュメントです。
Godot Engine向けのEffekseerプラグインについて説明します。
このプラグインをGodotプロジェクトに導入することで、Effekseerで作成したエフェクトをGodot Engine上で再生することができます。

## 動作環境

### Godot Engineバージョン
- Godot Engine 3.2.3 以降
- Godot Engine 4.xは現状サポートしていません。

### ターゲット対応状況

EffekseerのプラグインはGDNative(C++)で実装されています。
一般的な環境はサポートしていますが、それ以外のプラットフォームではユーザーがビルドする必要があります。

EffekseerForGodotのターゲットごとのサポート状況は以下の通りです。

| ターゲット | サポート | アーキテクチャ |
|-----------|:-------:|---------------|
| Windows   | ✅ | x86, x86_64 |
| macOS     | ✅ | x86_64, arm64 |
| Linux     | ✅ | x86, x86_64 |
| Android   | ✅ | armv7, arm64, x86, x86_64 |
| iOS       | ✅ | armv7, arm64, x86_64(Simulator) |
| HTML5     | ✅ | wasm32 |
| その他    | ❓ | ユーザー側でビルドが必要 | 


## 既知の問題
- Effekseer 1.60の高度描画パネルにある、ソフトパーティクル"を除いた"機能は現状動作しません。

## Todo
- 高度描画機能
