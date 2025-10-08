##CG2test　LC1A_27_ヨコヤマタダノブ
### 必須要素
- objとスプライトを表示し、ImGuiでSRTを変えれる

### 加点要素 
- 球の描画
- ライトの向き、色、輝度をImGuiで変更できるLambertain Reflectance
- ライトの向き、色、輝度をImGuiで変更できるHalf Lambert
- ImGuiによりLambertain Reflectance、Half Lambert、ライティングなしの三種類を切り替えることが出来る
- UVTransformを操作することが出来る。回転中心、拡大中心も調整できる。
- 複数のモデルを描画し、それぞれ独立してパラメータを操作できる
- Utah Teapotを描画させていただいている
- Soundの追加。現在実装されている機能は[Soundの読み込み][Sound再生(ループか単発か選択可能)][Sound停止][Soundの音量調整][マスター音量の調整][現在のSoundの音量の取得][現在のマスター音量の取得][現在そのSoundが再生されているかboolで取得]
- GamePadに対応
- Stanford Bunnyを描画させていただいている
- MultiMeshに対応している
- Suzanneを描画してあげている
- ImGuiにいろいろ追加しすぎてちょっと見にくい
- 新たなパイプラインを追加し、ワイヤーフレーム描画が可能になっている
- 使いやすさ、理解のしやすさを追い求めたらNoviceになっていた
- 親子関係を簡単に作れるようになった
- マウスとオブジェクトの当たり判定を取得する関数があったが親子関係のシステムと干渉してしまったらしく亡くなってしまった
- Drawobjの使いやすさは正直かなり自信がある。Drawobjを含むGame::RenderData_Modelは発展途上