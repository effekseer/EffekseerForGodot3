# ターゲット固有情報

## Mobile

モバイルターゲットでは次のEffekseerの機能は、デフォルト設定のままでは機能しません。

- 歪みシェーダ
- ソフトパーティクル

上記のエフェクトを機能させるためには、Godotの`プロジェクト設定`の`Quality`の`Framebuffer Allocation.mobile`に`3D`もしくは`2D`を設定する必要があります。

![](../img/Godot_mobile_framebuffer.png)

## HTML5
EffekseerプラグインをHTML5ターゲットで有効にするには`Export Type`を`GDNative`に設定する必要があります。

![](../img/Godot_export_html5.png)
