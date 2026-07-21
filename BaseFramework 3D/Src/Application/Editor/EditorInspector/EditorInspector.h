#pragma once

class EditorInspector
{
public:

	void  Draw();

private:




	// オブジェクトの名前変更
	void NameChange();

	// 座標変更
	void UpdatePos();
	// 大きさ変更
	void UpdateScale();

	// オブジェクト削除
	void Delete();

};