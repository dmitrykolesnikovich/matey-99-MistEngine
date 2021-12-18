#include "EntityDetailsPanel.h"

#include "imgui.h"
#include "Editor.h"
#include "Scene/Component/StaticMeshComponent.h"
#include "Scene/Component/InstanceRenderedMeshComponent.h"
#include "Scene/Component/Light/DirectionalLight.h"
#include "Scene/Component/Light/PointLight.h"
#include "Scene/Component/Light/SpotLight.h"

#include <glm/gtc/type_ptr.hpp>

bool Equals(float arr[3], glm::vec3 vec)
{
    return arr[0] == vec.x &&
           arr[1] == vec.y &&
           arr[2] == vec.z;
}

EntityDetailsPanel::EntityDetailsPanel(Ref<Editor> editor) : m_Editor(editor)
{
	m_Entity = Ref<Entity>();
}

void EntityDetailsPanel::Render()
{
	ImGui::Begin("Details");

    size_t maxSize = 128;
    char* name = (char*)m_Entity->m_Name.c_str();
    ImGui::InputText("##Name", name, maxSize);
    m_Entity->m_Name = name;
    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    ImGui::Text("Transform");
    float arr[3];
    arr[0] = m_Entity->GetTransform().LocalPosition.x;
    arr[1] = m_Entity->GetTransform().LocalPosition.y;
    arr[2] = m_Entity->GetTransform().LocalPosition.z;
    ImGui::DragFloat3("Position", arr, 0.5f);

    if (!Equals(arr, m_Entity->GetTransform().LocalPosition))
        m_Entity->SetLocalPosition({ arr[0], arr[1], arr[2] });

    arr[0] = m_Entity->GetTransform().LocalRotation.x;
    arr[1] = m_Entity->GetTransform().LocalRotation.y;
    arr[2] = m_Entity->GetTransform().LocalRotation.z;
    ImGui::DragFloat3("Rotation", arr, 1.0f);

    if (!Equals(arr, m_Entity->GetTransform().LocalRotation))
        m_Entity->SetLocalRotation({ arr[0], arr[1], arr[2] });

    arr[0] = m_Entity->GetTransform().LocalScale.x;
    arr[1] = m_Entity->GetTransform().LocalScale.y;
    arr[2] = m_Entity->GetTransform().LocalScale.z;
    ImGui::DragFloat3("Scale", arr, 0.1f);

    if (!Equals(arr, m_Entity->GetTransform().LocalScale))
        m_Entity->SetLocalScale({ arr[0], arr[1], arr[2] });

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    if (auto mesh = m_Entity->GetComponent<StaticMeshComponent>())
    {
        ImGui::Text("Static Mesh");

        std::string path = mesh->GetPath();
        std::string name = path.substr(path.find_last_of("/") + 1);
        if (ImGui::BeginCombo("Static Mesh", name.c_str()))
        {
            std::vector<std::string> extensions = std::vector<std::string>();
            extensions.push_back("obj");
            extensions.push_back("fbx");
            extensions.push_back("3ds");
            extensions.push_back("dae");
            DisplayResources(extensions);

            ImGui::EndCombo();
        }

        ImGui::Text("Materials");
        for (int i = 0; i < mesh->GetMaterials().size(); i++)
        {
            path = mesh->GetMaterialsPaths().at(i);
            name = path.substr(path.find_last_of("/") + 1);

            ImGui::PushID(i);
            if (ImGui::BeginCombo(("[" + std::to_string(i) + "]").c_str(), name.c_str()))
            {
                std::vector<std::string> extensions = std::vector<std::string>();
                extensions.push_back("mat");
                DisplayResources(extensions, i);

                ImGui::EndCombo();
            }
            ImGui::PopID();
        }
    }
    if (auto mesh = m_Entity->GetComponent<InstanceRenderedMeshComponent>())
    {
        ImGui::Text("Instance Rendered Mesh");

        std::string path = mesh->GetPath();
        std::string name = path.substr(path.find_last_of("/") + 1);
        if (ImGui::BeginCombo("Static Mesh", name.c_str()))
        {
            std::vector<std::string> extensions = std::vector<std::string>();
            extensions.push_back("obj");
            extensions.push_back("fbx");
            extensions.push_back("3ds");
            extensions.push_back("dae");
            DisplayResources(extensions);

            ImGui::EndCombo();
        }

        ImGui::Text("Materials");
        for (int i = 0; i < mesh->GetMaterials().size(); i++)
        {
            path = mesh->GetMaterialsPaths().at(i);
            name = path.substr(path.find_last_of("/") + 1);

            ImGui::PushID(i);
            if (ImGui::BeginCombo(("[" + std::to_string(i) + "]").c_str(), name.c_str()))
            {
                std::vector<std::string> extensions = std::vector<std::string>();
                extensions.push_back("mat");
                DisplayResources(extensions, i);

                ImGui::EndCombo();
            }
            ImGui::PopID();
        }
    }
    if (auto light = m_Entity->GetComponent<Light>())
    {
        ImGui::Text("Light");

        ImGui::Text("Light Type: ");
        const char* type = "";

        if (Cast<DirectionalLight>(light))
            type = "Directional";
        else if (Cast<PointLight>(light))
            type = "Point";
        else if (Cast<SpotLight>(light))
            type = "Spot";

        ImGui::SameLine();
        if (ImGui::BeginMenu(type))
        {
            if (ImGui::MenuItem("Directional"))
            {
                if (!Cast<DirectionalLight>(light))
                {
                    m_Entity->RemoveComponent<Light>();
                    m_Entity->AddComponent<DirectionalLight>(m_Entity->m_Scene->m_LightsVertexUniformBuffer, m_Entity->m_Scene->m_LightsFragmentUniformBuffer);
                }
            }
            if (ImGui::MenuItem("Point"))
            {
                if (!Cast<PointLight>(light))
                {
                    m_Entity->RemoveComponent<Light>();
                    m_Entity->AddComponent<PointLight>(m_Entity->m_Scene->m_LightsVertexUniformBuffer, m_Entity->m_Scene->m_LightsFragmentUniformBuffer);
                }
            }
            if (ImGui::MenuItem("Spot"))
            {
                if (!Cast<SpotLight>(light))
                {
                    m_Entity->RemoveComponent<Light>();
                    m_Entity->AddComponent<SpotLight>(m_Entity->m_Scene->m_LightsVertexUniformBuffer, m_Entity->m_Scene->m_LightsFragmentUniformBuffer);
                }
            }

            ImGui::EndMenu();
        }

        ImGui::DragFloat3("Color", (float*)&light->m_Color, 0.1f, 0.0f, 100.0f);
        ImGui::Checkbox("Cast Shadows", &light->m_ShadowsEnabled);

        if (auto spotLight = m_Entity->GetComponent<SpotLight>())
        {
            float temp = spotLight->m_InnerCutOff;
            ImGui::DragFloat("Inner Cut Off", &temp, 0.01f, 0.0f, 1.0f);
            if (temp != spotLight->m_InnerCutOff)
                spotLight->SetInnerCutOff(temp);

            temp = spotLight->m_OuterCutOff;
            ImGui::DragFloat("Outer Cut Off", &temp, 0.01f, 0.0f, 1.0f);
            if (temp != spotLight->m_OuterCutOff)
                spotLight->SetOuterCutOff(temp);
        }
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
    }

    bool addComponent = false;
    bool staticMesh = false;
    bool instanceRenderedMesh = false;
    bool dirLight = false;
    bool pointLight = false;
    bool spotLight = false;
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::BeginMenu("Add Component"))
        {
            ImGui::MenuItem("Static Mesh", "", &staticMesh);
            ImGui::MenuItem("Instance Rendered Mesh", "", &instanceRenderedMesh);
            if (ImGui::BeginMenu("Light"))
            {
                ImGui::MenuItem("Directional Light", "", &dirLight);
                ImGui::MenuItem("Point Light", "", &pointLight);
                ImGui::MenuItem("Spot Light", "", &spotLight);

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        ImGui::EndPopup();
    }

    if (staticMesh)
        m_Entity->AddComponent<StaticMeshComponent>();
    if (instanceRenderedMesh)
        m_Entity->AddComponent<InstanceRenderedMeshComponent>();
    if (dirLight)
        m_Entity->AddComponent<DirectionalLight>(m_Entity->m_Scene->m_LightsVertexUniformBuffer, m_Entity->m_Scene->m_LightsFragmentUniformBuffer);
    if (pointLight)
        m_Entity->AddComponent<PointLight>(m_Entity->m_Scene->m_LightsVertexUniformBuffer, m_Entity->m_Scene->m_LightsFragmentUniformBuffer);
    if (spotLight)
        m_Entity->AddComponent<SpotLight>(m_Entity->m_Scene->m_LightsVertexUniformBuffer, m_Entity->m_Scene->m_LightsFragmentUniformBuffer);

    if (ImGui::Button("Close"))
    {
        m_Editor->HideDetails();
        m_Editor->GetSceneHierarchyPanel()->UnselectEntity();
    }

	ImGui::End();
}

void EntityDetailsPanel::DisplayResources(std::vector<std::string> extensions, int index)
{
    for (auto& p : std::filesystem::recursive_directory_iterator("../../res"))
    {
        std::stringstream ss;
        ss << p.path();
        std::string path = ss.str();
        CorrectPath(path);
        std::string name = path.substr(path.find_last_of("/") + 1);
        std::string ext = name.substr(name.find_first_of('.') + 1);

        for (auto extension : extensions)
        {
            if (extension == ext)
            {
                const bool isSelected = false;
                if (ImGui::Selectable(name.c_str(), &isSelected))
                {
                    if (ext == "obj" || ext == "fbx" || ext == "3ds" || ext == "dae")
                    {
                        auto mesh = m_Entity->GetComponent<StaticMeshComponent>();
                        mesh->ChangeMesh(path);
                    }
                    else if (ext == "mat")
                    {
                        auto mesh = m_Entity->GetComponent<StaticMeshComponent>();
                        mesh->ChangeMaterial(index, path);
                    }
                }
            }
        }
    }

}

void EntityDetailsPanel::CorrectPath(std::string& path)
{
    path = path.substr(path.find_first_of('"') + 1, path.length() - 2);

    std::size_t index = 0;
    while (true)
    {
        index = path.find("\\\\", index);
        if (index == std::string::npos)
            break;

        path.replace(index, 2, "/");
    }
}
