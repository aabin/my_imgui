#pragma once
#include "ft_base.h"
class ft_polygon_image :
	public ft_base
{
	vector<ft_vertex>  _vertexes;
public:
	void draw();
#if !defined(IMGUI_WAYLAND)
	void draw_peroperty_page();
#endif
	bool init_from_json(Value& jvalue);
	bool handle_mouse();
};
REGISTER_CONTROL(ft_polygon_image)
