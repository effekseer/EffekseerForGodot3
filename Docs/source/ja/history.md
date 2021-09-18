# 更新履歴

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
- EffekseerForGodot最初のリリース
