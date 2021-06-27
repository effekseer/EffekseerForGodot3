# APIリファレンス

## EffekseerEmitter

**継承**: Spatial < Node < Object

Effekseerエフェクトの3D放出源。

### 説明

Effekseerのエフェクトを再生、描画するための3Dオブジェクトです。EffekseerEffectをセットし、play()することで再生することができます。また、設定されたトランスフォーム（位置、回転、拡大縮小）はエフェクトの描画に反映されます。

### プロパティ一覧

#### EffekseerEffect effect

|           |                   |
|-----------|-------------------|
| *Setter*	| set_effect(value) |
| *Getter*	| get_effect()      |

エミッターに設定されたエフェクト。

----

#### bool autoplay

|           |                     |
|-----------|---------------------|
| *Setter*	| set_autoplay(value) |
| *Getter*	| is_autoplay()       |

自動再生の設定。

- true: 自動再生を行う
- false: 自動再生を行わない

----

#### bool paused

|           |                   |
|-----------|-------------------|
| *Setter*	| set_paused(value) |
| *Getter*	| is_paused()       |

一時停止の設定。

- true: 一時停止する
- false: 一時停止しない

----

#### bool speed

|           |                   |
|-----------|-------------------|
| *Setter*	| set_speed(value)  |
| *Getter*	| get_speed()       |

再生速度の設定。範囲:0.0 ～ 10.0

- 0.5: 1/2倍速再生
- 2.0: 2倍速再生

----

#### Color color

|           |                   |
|-----------|-------------------|
| *Setter*	| set_color(value)  |
| *Getter*	| get_color()       |

描画色の設定。エフェクトが持つ色と掛け合わされます。

----

### メソッド一覧

#### void play()
再生開始します。

----

#### void stop()
再生停止します。

----

#### void stop_root()
ルートノードを停止します。

----

#### bool is_playing()
再生状況を取得します。

- true: 再生中
- false: 未再生、もしくは再生終了

----

### シグナル一覧

#### finished()
エフェクトの再生終了時に発生します。

----

## EffekseerEmitter2D

**継承**: Node2D < CanvasItem <  Node < Object

Effekseerエフェクトの2D放出源。

### 説明

Effekseerのエフェクトを再生、描画するための2Dオブジェクトです。EffekseerEffectをセットし、play()することで再生することができます。また、設定されたトランスフォーム（位置、回転、拡大縮小）はエフェクトの描画に反映されます。

### プロパティ一覧

#### EffekseerEffect effect

|           |                   |
|-----------|-------------------|
| *Setter*	| set_effect(value) |
| *Getter*	| get_effect()      |

エミッターに設定されたエフェクト。

----

#### bool autoplay

|           |                     |
|-----------|---------------------|
| *Setter*	| set_autoplay(value) |
| *Getter*	| is_autoplay()       |

自動再生の設定。

- true: 自動再生を行う
- false: 自動再生を行わない

----

#### bool paused

|           |                   |
|-----------|-------------------|
| *Setter*	| set_paused(value) |
| *Getter*	| is_paused()       |

一時停止の設定。

- true: 一時停止する
- false: 一時停止しない

----

#### bool speed

|           |                   |
|-----------|-------------------|
| *Setter*	| set_speed(value)  |
| *Getter*	| get_speed()       |

再生速度の設定。範囲:0.0 ～ 10.0

- 0.5: 1/2倍速再生
- 2.0: 2倍速再生

----

#### Color color

|           |                   |
|-----------|-------------------|
| *Setter*	| set_color(value)  |
| *Getter*	| get_color()       |

描画色の設定。エフェクトが持つ色と掛け合わされます。

#### Vector3 orientation

|           |                   |
|-----------|-------------------|
| *Setter*	| set_orientation(value)  |
| *Getter*	| get_orientation()       |

エフェクトの3D向きの設定。Transform2Dのrotationの前に掛けられます。

----

### メソッド一覧

#### void play()
再生開始します。

----

#### void stop()
再生停止します。

----

#### void stop_root()
ルートノードを停止します。

----

#### bool is_playing()
再生状況を取得します。

- true: 再生中
- false: 未再生、もしくは再生終了

----

### シグナル一覧

#### finished()
エフェクトの再生終了時に発生します。

----

## EffekseerEffect

**継承**: Resource < Reference < Object

Effekseerのエフェクトリソース。

### プロパティ一覧

#### String data_path

|           |                        |
|-----------|------------------------|
| *Setter*	| set_data_path(value)  |
| *Getter*	| get_data_path()       |

ロードしたエフェクトファイルのパス。

基本的には変更しないでください。

----

#### PoolByteArray data_bytes

|           |                        |
|-----------|------------------------|
| *Setter*	| set_data_bytes(value)  |
| *Getter*	| get_data_bytes()       |

ロードしたエフェクトファイルのバイトデータ。

基本的には変更しないでください。

----

#### Array subresources

|           |                          |
|-----------|--------------------------|
| *Setter*	| set_subresources(value)  |
| *Getter*	| get_subresources()       |

ロードしたエフェクトの依存リソース。

基本的には変更しないでください。

----

### メソッド一覧

#### void load(String path)
ファイルパス指定でエフェクトのロードを行います。

基本的にはこのメソッドは使わず、Godotのリソースロードを使用してください。

----

#### void release()
エフェクトをリリースします。

基本的にはこのメソッドは使わず、Godotのリソース機能でリリースしてください。

----

## EffekseerSystem

**継承**: Node < Object

Effekseerのシステム管理用シングルトン。

### メソッド一覧

#### void stop_all_effects()
現在再生中の全てのエフェクトを停止します。

----

#### void set_paused_to_all_effects()
現在再生中の全てのエフェクトにポーズ設定を行います。

----

#### int get_total_instance_count()
現在利用中のインスタンス数を取得します。

----
