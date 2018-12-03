#include "ft_image_play.h"
#include "res_output.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "common_functions.h"
/*
x'=(x-a)cos��+(y-b)sin��+a
y'=-(x-a)sin��+(y-b)cos��+b
*/
namespace auto_future
{
	ft_image_play::ft_image_play()
		:ft_base()
	{
		memset(_img_pt._texture_name, 0, FILE_NAME_LEN);
		memset(_img_pt._texture_fmt_name, 0, FILE_NAME_LEN);
		reg_property_handle(&_img_pt, 6, [this](void*){
			int frame_cnt = get_frames_count();
			ImGui::SliderInt("frame index:", &_img_pt._frame_index, 0, frame_cnt);
		});
	}
	void ft_image_play::draw()
	{
		if (!_texture || !_vtexture_cd.size())
		{
			return;
		}
		ft_base::draw();
		vres_txt_cd& ptext_cd = _vtexture_cd;
		if (_img_pt._frame_index >= ptext_cd.size())
		{
			printf("invalid texture index:%d\n", _img_pt._frame_index);
			return;

		}
		int texture_id =_texture->_txt_id;
		int texture_width = _texture->_width;
		int texture_height = _texture->_height;
		float sizew = _img_pt._sizew;
		float sizeh = _img_pt._sizeh;
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
		ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
		ImVec2 pos4 = { pos1.x + sizew, pos1.y };

		ImVec2 uv0 = ImVec2(ptext_cd[_img_pt._frame_index]._x0 / texture_width, ptext_cd[_img_pt._frame_index]._y0 / texture_height);
		ImVec2 uv1 = ImVec2(ptext_cd[_img_pt._frame_index]._x0 / texture_width, (ptext_cd[_img_pt._frame_index]._y1) / texture_height);
		ImVec2 uv2 = ImVec2((ptext_cd[_img_pt._frame_index]._x1) / texture_width, (ptext_cd[_img_pt._frame_index]._y1) / texture_height);
		ImVec2 uv3 = ImVec2((ptext_cd[_img_pt._frame_index]._x1) / texture_width, (ptext_cd[_img_pt._frame_index]._y0) / texture_height);

		float offsetx = abpos.x - base_pos().x;
		float offsety = abpos.y - base_pos().y;
		ImVec2 axisBasePos = { offsetx + _img_pt._aposx + winpos.x, offsety + _img_pt._aposy + winpos.y };
		if (_img_pt._angle_nml != 0.f)
		{
			pos1 = rotate_point_by_zaxis(pos1, _img_pt._angle_nml, axisBasePos);
			pos2 = rotate_point_by_zaxis(pos2, _img_pt._angle_nml, axisBasePos);
			pos3 = rotate_point_by_zaxis(pos3, _img_pt._angle_nml, axisBasePos);
			pos4 = rotate_point_by_zaxis(pos4, _img_pt._angle_nml, axisBasePos);
		}

		ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
		if (is_selected())//draw envelope
		{
			ImU32 col = ImGui::GetColorU32(ImGuiCol_HeaderActive);
			ImVec2 editunit(edit_unit_len, edit_unit_len);
			ImVec2 pos1a = pos1 - editunit;
			ImVec2 pos1b = pos1 + editunit;
			ImGui::RenderFrame(pos1a, pos1b, col);
			ImVec2 pos2a = pos2 - editunit;
			ImVec2 pos2b = pos2 + editunit;
			ImGui::RenderFrame(pos2a, pos2b, col);
			ImVec2 pos3a = pos3 - editunit;
			ImVec2 pos3b = pos3 + editunit;
			ImGui::RenderFrame(pos3a, pos3b, col);
			ImVec2 pos4a = pos4 - editunit;
			ImVec2 pos4b = pos4 + editunit;
			ImGui::RenderFrame(pos4a, pos4b, col);
			ImVec2 posaa = axisBasePos - editunit;
			ImVec2 posab = axisBasePos + editunit;
			ImGui::RenderFrame(posaa, posab, col);

		}
#endif

	}
	void ft_image_play::link()
	{
		auto atex = g_mtexture_list.find(_img_pt._texture_name);
		if (atex != g_mtexture_list.end())
		{
			_texture = atex->second;
		}
		auto atxt_fmt = g_mfiles_list.find(_img_pt._texture_fmt_name);
		if (atxt_fmt != g_mfiles_list.end())
		{
			get_txt_uv_vector((char*)atxt_fmt->second->_pbin, _vtexture_cd);

		}
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	/*
	fields:
	screen_pos,
	texture_id_index
	texture_index
	*/
	bool ft_image_play::init_from_json(Value& jvalue)
	{
		ft_base::init_from_json(jvalue);
		//g_cur_texture_id_index = jvalue["texture_id_index"].asInt();
		strcpy(_img_pt._texture_name, jvalue["texture name"].asCString());
		strcpy(_img_pt._texture_fmt_name, jvalue["texture format"].asCString());
		_img_pt._frame_index = jvalue["frame_index"].asInt();
		Value& jsize = jvalue["size"];
		if (!jsize.isNull())
		{
			_img_pt._sizew = jsize["w"].asDouble();
			_img_pt._sizeh = jsize["h"].asDouble();
		}
		/*if (_img_pt._size.x == 0.f || _img_pt._size.y == 0.f)
		{
			vres_txt_cd& ptext_cd = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
			_img_pt._size.x = ptext_cd[_img_pt._texture_index]._x1 - ptext_cd[_img_pt._texture_index]._x0;
			_img_pt._size.y = ptext_cd[_img_pt._texture_index]._y1 - ptext_cd[_img_pt._texture_index]._y0;
		}*/
		Value& jaxispos = jvalue["axipos"];
		if (!jaxispos.isNull())
		{
			_img_pt._aposx = jaxispos["x"].asDouble();
			_img_pt._aposy = jaxispos["y"].asDouble();
			_img_pt._angle_nml = jaxispos["angle"].asDouble();
		}
		else
		{
			float aw = _img_pt._sizew / 2;
			float ah = _img_pt._sizeh / 2;
			_img_pt._aposx = base_pos().x + aw;
			_img_pt._aposy = base_pos().y + ah;
		}
		link();
		return true;
	}
	bool ft_image_play::init_json_unit(Value& junit)
	{
		ft_base::init_json_unit(junit);
		junit["texture name"] = _img_pt._texture_name;
		junit["texture format"] = _img_pt._texture_fmt_name;
		junit["frame_index"] = _img_pt._frame_index;
		Value jsize(objectValue);
		jsize["w"] = _img_pt._sizew;
		jsize["h"] = _img_pt._sizeh;
		junit["size"] = jsize;
		Value jaxispos(objectValue);
		jaxispos["x"] = _img_pt._aposx;
		jaxispos["y"] = _img_pt._aposy;
		jaxispos["angle"] = _img_pt._angle_nml;
		junit["axipos"] = jaxispos;
		return true;
	}

#endif
}
