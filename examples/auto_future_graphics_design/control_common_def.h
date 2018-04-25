#pragma once
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <vector>
#include <string>
#include "json/json.h"
#include <algorithm>
//#include <map>
struct ft_vertex
{

	ImVec4  pos;
	ImVec2  uv;
	ImU32   col;
};
typedef unsigned short ushort;
using namespace std;
using namespace Json;
class base_ui_component;
const unsigned char name_len = 0xff;
#if !defined(IMGUI_WAYLAND)
const float edit_unit_len = 5.0f;
const float imge_edit_view_width = 300.f;
#endif
extern base_ui_component* find_a_uc_from_uc(base_ui_component& tar_ui, const char* uname);
class base_ui_component
{
	friend base_ui_component* find_a_uc_from_uc(base_ui_component& tar_ui, const char* uname);
protected:
	string _name;
	ImVec3 _pos;
	vector<base_ui_component*> _vchilds;
	base_ui_component* _parent;
	bool _visible;
#if !defined(IMGUI_WAYLAND)
	char _name_bk[name_len];
protected:
	bool _selected;
public:
	virtual void draw_peroperty_page() = 0;
	bool is_selected()
	{
		return _selected;
	}
	void set_selected(bool beselected)
	{
		_selected = beselected;
	}
#endif
public:
	static float screenw;
	static float screenh;
	virtual void draw() = 0;
	virtual base_ui_component* get_a_copy() = 0;
	//virtual base_ui_component* get_new_instance() = 0;
	void set_name(string& name)
	{
		_name = name; 
#if !defined(IMGUI_WAYLAND)
		memset(_name_bk, 0, name_len);
		strcpy(_name_bk, _name.c_str());
#endif
	}
	string& get_name(){ return _name; }
	virtual bool handle_mouse(){ return false; }
	virtual bool init_from_json(Value&){ return true; }
	virtual bool init_json_unit(Value&){ return true; };
	base_ui_component()
		:_visible(true)
		, _parent(NULL)
#if !defined(IMGUI_WAYLAND)
		, _selected(false)
#endif
	{

	}
	
	~base_ui_component()
	{
		for (auto it:_vchilds)
		{
			delete it;
		}
	}
	void add_child(base_ui_component* pchild){ pchild->_parent = this; _vchilds.push_back(pchild); }
	void remove_child(base_ui_component* pchild)
	{
		auto it = find(_vchilds.begin(), _vchilds.end(), pchild);
		if (it!=_vchilds.end())
		{
			_vchilds.erase(it);
			delete pchild;
		}
	}
	base_ui_component* get_child(int index)
	{
		return _vchilds[index];
	}
	
	size_t get_child_count(){ return _vchilds.size(); }
	base_ui_component* get_parent(){ return _parent; }
	size_t child_count(){ return _vchilds.size(); }
	ImVec3 base_pos(){ return _pos; }
	ImVec3 absolute_coordinate_of_base_pos()
	{
		ImVec3 base_pos = _pos;
		base_ui_component* parent = get_parent();
		while (parent)
		{
			ImVec3 pbase_pos = parent->base_pos();
			base_pos += pbase_pos;
			parent = parent->get_parent();
		}
		return base_pos;
	}
	void offset(ImVec2& imof)
	{
		_pos.x += imof.x;
		_pos.y += imof.y;
	}
	bool is_visible(){ return _visible; }
	void set_visible(bool visible){ _visible = visible; }

};
