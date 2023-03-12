# 更新履歴

## 1.70
- 変更箇所
  - Effekseer 1.70へ更新
  - Emitter/Emitter2Dにメソッド`set_dynamic_parameter`を追加
  - Emitter/Emitter2Dにメソッド`send_trigger`を追加

## 1.62b
- 不具合修正
  - 再インポートに失敗する不具合を修正 #8
  - マテリアルの色空間が正しくない問題を修正

## 1.62a
- 変更箇所
  - Effekseer 1.62aへ更新
  - Godot-cpp 3.4へ更新
  - エフェクトプレビューのインスペクタプラグインを追加
  - `get_tree().paused`に対応
  - Emitter/Emitter2Dの`visibility`をエフェクトの表示に反映するように変更
  - Emitter/Emitter2Dのupdate/renderループを効率化
  - EffectResourceのサイズを最適化（不要なデータの削減と圧縮を適用）
  - EffekseerSpatialGizmoPluginに名前を付けた
- 不具合修正
  - Emitterのplay()時に`target_position`が適用されない不具合を修正

## 1.61c
- 変更箇所
  - Effekseer 1.61cへ更新
  - dll/so/dylib/wasm等バイナリファイルのサイズを削減
  - Emitter/Emitter2Dにシグナル `finished` を追加
  - Emitter/Emitter2Dにプロパティ `target_position` を追加
- 不具合修正
  - カスタムマテリアルで深度テクスチャを使用すると実行時エラーになる問題を修正
  - Emitter/Emitter2Dの `effect` にnullを指定するとクラッシュする問題を修正
  - 色空間が正しくない問題を修正

## 1.60 Beta3
- Effekseer 1.60ブランチの最新へ更新

## 1.60 Beta2
- 不具合修正
  - 2Dの座標変換計算を修正
  - 2Dサウンド再生でクラッシュする問題を修正
  - 高FPS時にエフェクトの更新が行われない不具合を修正 (#1)

## 1.60 Beta1
- EffekseerForGodot3最初のリリース
