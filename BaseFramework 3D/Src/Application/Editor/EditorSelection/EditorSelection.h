#pragma once

class EditorSelection
{
public:

	EditorSelection() = default;
	~EditorSelection() = default;


	// 現在選択されているオブジェクト
	void SetSelectedObject(const std::shared_ptr<KdGameObject>& obj)
	{
		// 前に選択されていたフラグをfalse
		if (m_spSelectedObject) { m_spSelectedObject->SetSelected(false); }

		m_spSelectedObject = obj;

		// 新たに選択されたフラグをtrueにする
		if (m_spSelectedObject) { m_spSelectedObject->SetSelected(true); }
	}

	// 選択中のオブジェクトを返す
	const std::shared_ptr<KdGameObject>& GetSelectedObject() const
	{
		return m_spSelectedObject;
	}


private:

	std::shared_ptr<KdGameObject>m_spSelectedObject = nullptr;

};