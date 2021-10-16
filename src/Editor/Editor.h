#pragma once

#include "typedefs.h"
#include "SceneHierarchyPanel.h"
#include "EntityDetailsPanel.h"
#include "ContentBrowserPanel.h"
#include "MaterialEditorPanel.h"

#include <ImGuizmo.h>
#include <mutex>

class Editor : public std::enable_shared_from_this<Editor>
{
private:
	static Ref<Editor> s_Instance;
	static std::mutex s_Mutex;

	Ref<SceneHierarchyPanel> m_SceneHierarchyPanel;
	Ref<EntityDetailsPanel> m_EntityDetailsPanel;
	Ref<ContentBrowserPanel> m_ContentBrowserPanel;
	Ref<MaterialEditorPanel> m_MaterialEditorPanel;

	bool m_DetailsPanel;
	bool m_MaterialEditor;
	ImGuizmo::OPERATION m_Operation;

public:
	Editor();
	~Editor();

	Editor(Editor& other) = delete;
	void operator=(const Editor&) = delete;

	static Ref<Editor> GetInstance();

	void Initialize(Ref<Scene> scene);
	void Update();

	void ShowDetails(Ref<Entity> entity);
	void HideDetails();

	void ShowMaterialEditor(Ref<Material> material);
	void HideMaterialEditor();

	inline ImGuizmo::OPERATION GetGizmoOperation() const { return m_Operation; }
	inline void SetGizmoOperation(ImGuizmo::OPERATION operation) { m_Operation = operation; }

	inline Ref<SceneHierarchyPanel> GetSceneHierarchyPanel() const { return m_SceneHierarchyPanel; }

private:
	Ref<Editor> GetReference();
};