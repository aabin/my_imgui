#include "project_edit.h"
#include "user_control_imgui.h"

/*
1�������ĩ����flags|leaf������flags|openonarrow
2������node����selectable,���ctrl�����ϴε�node��selected״̬������˴�!ctrl�������ϴε�selected node״̬
3�����ж��node����selected״̬����ݲ˵�ֻ��copy��addsiblings��ִ��add siblingʱ����ĩ���򸸼�Ѱ��selected��node��ִ�и�����Ϊ
4��root��ѡ��ʱ������ִ��addsibling
*/
base_ui_component* psel = NULL;
base_ui_component* project_edit(base_ui_component& fb)
{
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;// | ImGuiTreeNodeFlags_Selected;
	string cname = typeid(fb).name();
	cname = cname.substr(sizeof("class"));
	string& objname = fb.get_name();
	bool beparent = fb.get_child_count() > 0;
	if (!beparent)
	{
		node_flags_root |= ImGuiTreeNodeFlags_Leaf;
	}
	if (fb.is_selected())
	{
		node_flags_root |= ImGuiTreeNodeFlags_Selected;
	}
	if (IconTreeNode(cname,objname.c_str(), node_flags_root))
	{
		if (ImGui::IsItemClicked())
		{
		    fb.set_selected(true);
			if (psel)
			{
				psel->set_selected(false);
			}
			psel = &fb;
		}
		if (beparent)
		{
			for (size_t ix = 0; ix < fb.get_child_count(); ix++)
			{
				base_ui_component* pchild = fb.get_child(ix);
				psel=project_edit(*pchild);
			}
		}
		
		ImGui::TreePop();

	}
	
	if (fb.get_parent()==NULL)
	{
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("copy", NULL, false))
			{
			}

			if (ImGui::BeginMenu("add child"))
			{
				if (ImGui::MenuItem("base", NULL, false))
				{
				}
				if (ImGui::MenuItem("image", NULL, false))
				{
				}
				if (ImGui::MenuItem("button", NULL, false))
				{
				}
				if (ImGui::MenuItem("listbox", NULL, false))
				{
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("add sibling", NULL, false))
			{
			}

			ImGui::EndPopup();
		}
	}
	return psel;
}
