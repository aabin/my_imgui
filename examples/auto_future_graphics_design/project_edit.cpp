#include "project_edit.h"
#include "user_control_imgui.h"

/*
1�������ĩ����flags|leaf������flags|openonarrow
2������node����selectable,���ctrl�����ϴε�node��selected״̬������˴�!ctrl�������ϴε�selected node״̬
3�����ж��node����selected״̬����ݲ˵�ֻ��copy��addsiblings��ִ��add siblingʱ����ĩ���򸸼�Ѱ��selected��node��ִ�и�����Ϊ
4��root��ѡ��ʱ������ִ��addsibling
*/
void project_edit::view_object(base_ui_component& fb)
{
	ImGuiTreeNodeFlags node_flags_root = ImGuiTreeNodeFlags_DefaultOpen;// | ImGuiTreeNodeFlags_Selected;
	string cname = typeid(fb).name();
	cname = cname.substr(sizeof("class"));
	string objname = fb.get_name();
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
			if (_pcurrent_object)
			{
				_pcurrent_object->set_selected(false);
			}
			_pcurrent_object = &fb;
		}
		if (beparent)
		{
			for (size_t ix = 0; ix < fb.get_child_count(); ix++)
			{
				base_ui_component* pchild = fb.get_child(ix);
				view_object(*pchild);
			}
		}
		ImGui::TreePop();
	}
}

void project_edit::objects_view()
{
	view_object(_root);
}

void project_edit::popup_context_menu()
{
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("copy", NULL, false))
		{
			_pcopy_object = _pcurrent_object;
		}
		if (ImGui::MenuItem("paste",NULL,false))
		{
			if (_pcopy_object&&_pcurrent_object)
			{
				_pcurrent_object->add_child(get_copy_of_object(_pcopy_object));
			}
		}
		if (ImGui::BeginMenu("add child"))
		{
			factory::get().iterate_types([this](string cname, function<base_ui_component*()> infun){
				if (ImGui::MenuItem(cname.c_str(), NULL, false))
				{
					if (_pcurrent_object)
					{
						base_ui_component* pchild = infun();
						pchild->set_name(cname);
						_pcurrent_object->add_child(pchild);
					}
				}
			});
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("add sibling", NULL, false))
		{
			if (_pcurrent_object&&_pcurrent_object->get_parent())
			{
				base_ui_component* psibling = get_copy_of_object(_pcurrent_object);
				_pcurrent_object->get_parent()->add_child(psibling);
			}
			//ImGui::EndMenu();
		}
		if (ImGui::MenuItem("delete",NULL,false))
		{
			if (_pcurrent_object)
			{

				base_ui_component* pparent = _pcurrent_object->get_parent();
				if (pparent)
				{

					pparent->remove_child(_pcurrent_object);
					_pcurrent_object = pparent;
				}
			}
		}

		ImGui::EndPopup();
	}
}
