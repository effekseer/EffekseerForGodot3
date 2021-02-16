# プラグインの使い方

## プラグインの導入

パッケージ内のaddonsディレクトリをGodotプロジェクト直下にコピーします。

![](../img/Godot_addons.png)

プロジェクト設定のプラグインタブから**Effekseer for Godot**の**Status**を有効にします。

![](../img/Godot_plugins.png)

以上でプラグインの導入は完了です。

## リソースファイル

Godotのプロジェクトの`res://`の中にエフェクトファイル(*.efkefc)やテクスチャ、サウンド、マテリアル等を配置します。

![](../img/Godot_resources.png)

### エフェクトのインポート
efkefcファイルはGodotのリソースとしてインポートされます。

![](../img/Godot_import_efkefc_success.png)

成功すると上図のようにテクスチャ、モデル、マテリアル等のサブリソースがリンクされます。

![](../img/Godot_import_efkefc_fail.png)

サブリソースがリンク出来ていない場合は、再インポートを試してください。

![](../img/Godot_import_efkefc.png)

#### scale設定
読み込んだエフェクトの大きさが小さく、サイズを調整したい時があります。その場合はefkefcファイルを選択し、インポートタブの`scale` のパラメーターを変更して`再インポート`ボタンを押します。

`EffekseerEmitter`のScaleを変更することでエフェクトの大きさを変更することもできますが、この方法だとエフェクトの設定によっては拡大されないことがあります。

## エミッタ
エミッタはエフェクトの発生源で、3Dシーン中に配置して使用します。

インポートしたエフェクトファイルを再生するには、ノードツリーに`EffekseerEmitter`がアタッチされた`Node`を追加します。

![](../img/Godot_emitter_nodetree.png)

次に`Effect`プロパティにインポートしたエフェクトをドラッグ&ドロップして指定します。

![](../img/Godot_emitter_scene.png)

`Autoplay`がチェックされているので、シーンを実行するとエフェクトが表示されます。

![](../img/Godot_emitter_play.png)

### エミッタのインスペクタ

![](../img/Godot_emitter_inspector.png)

| 項目     | 説明                                         |
|----------|---------------------------------------------|
| Effect   | エミッタで再生するエフェクトリソースを設定します |
| Autoplay | チェックを入れると、シーン開始時(`_ready()`のタイミング)に自動的にエフェクトを再生します |
| Paused   | 再生中のポーズ状態を設定します |
| Speed    | エフェクトの再生速度を設定します |
| Color    | エフェクトの色を設定します |

## プロジェクト設定

Effekseerの全体的な設定はGodotのプロジェクト設定から行うことができます。

![](../img/Godot_project_settings.png)

| 項目               | 説明                    |
|--------------------|------------------------|
| Instance Max Count | ノードが生成するインスタンスの同時最大数 |
| Square Max Count   | 描画に使用する四角形の同時最大数 |
| Draw Max Count     | ドローコールの同時最大数 |
| Sound Script       | サウンド再生で使われるスクリプト。差し替えが可能 |

