#include <fstream>
#include <string>
#include "ft_slider.h"
#include "res_output.h"
#include "common_functions.h"

namespace auto_future
{

	template<class T>
	inline T qu1et_square(T val) {
		return val * val;
	}
#define qu1et_thr_length(a, b) sqrt(qu1et_square<float>(a) + qu1et_square<float>(b))
	//计算单个四边形参与周长运算的长度
#define qu1et_one_quadrangle_length(a, b, c, d) (qu1et_thr_length(d.x-a.x, d.y-a.y) + qu1et_thr_length(b.x-c.x, b.y-c.y))/2

	//计算小矩形中上下的点坐标
#define qu1et_top_point_pos_x_of_square(a, b, c, d, l, h)  l*abs(d.x - a.x)/h + a.x
#define qu1et_top_point_pos_y_of_square(a, b, c, d, l, h) l*abs(d.y - a.y)/h + a.y
#define qu1et_bottom_point_pos_x_of_square(a, b, c, d, l, h) l*abs(c.x - b.x)/h + b.x
#define qu1et_bottom_point_pos_y_of_square(a, b, c, d, l, h) l*abs(c.y - b.y)/h + b.y

#define ARRAY_COUNT_DIRECTION_ITEM 3
	const char *const direction_iitem[ARRAY_COUNT_DIRECTION_ITEM] = { "x", "y", "custom" };

	/*
	 * @brief: g_cur_texture_id_index:
	 *	In order to realize the effect of skin change in the later period, current value is 0
	 *	the variate can get id value of the big mix texture
	 */

	ft_slider::ft_slider()
		:ft_base(), _slider_pt()
	{
		memset(&_slider_pt._cbuffer_random_text, 0, sizeof(_slider_pt._cbuffer_random_text));
	}

	bool ft_slider::read_point_position_file(const char *str)
	{
		auto ij = g_mfiles_list.find(str);
		if (ij == g_mfiles_list.end())
		{
			return  false;
		}
		//printf("*****file size %d\n", ij->second->_fsize);

		char *_pData = (char *)ij->second->_pbin;

		string line;
		int i = 0;
		while (i<ij->second->_fsize)
		{
			if (_pData[i] != '\n')
			{
				line += _pData[i];
			}
			else
			{
				int n, n1, n2;
				if (line.npos == (n = line.find('/'))) break;
				//printf("lin.length:%d\n", line.length());
				std::string str1 = line.substr(0, n);
				//printf("str1:%s\n", str1.c_str());
				std::string str2 = line.substr(n + 1, line.length() - n - 1);
				//printf("str2:%s\n", str2.c_str());
				if (str1.npos == (n1 = str1.find(','))) break;
				if (str2.npos == (n2 = str2.find(','))) break;

				ImVec2 _top_point = { (float)atof(str1.substr(0, n1).c_str()), (float)atof(str1.substr(n1 + 1, str1.length()).c_str()) };
				//printf("top:(%f,%f)\n", _top_point.x, _top_point.y);
				ImVec2 _bottom_point = { (float)atof(str2.substr(0, n2).c_str()), (float)atof(str2.substr(n2 + 1, str2.length()).c_str()) };
				//printf("bottom:(%f,%f)\n", _bottom_point.x, _bottom_point.y);
				ft_slider_random_point_vec.emplace_back(_top_point, _bottom_point);

				line.clear();
			}
			i++;
		}

#if 0 //注释读取文件，文本内容从文件列表中读取
		std::ifstream file(str);
		if (!file)
		{
			return false;
		}
		ft_slider_random_point_vec.clear();
		_random_all_length = 0.f;
		std::string line;
		while (getline(file, line))
		{
			int n, n1, n2;
			if (line.npos == (n = line.find('/'))) return false;

			std::string str1 = line.substr(1, n - 2);
			std::string str2 = line.substr(n + 2, line.length() - n - 3);

			if (str1.npos == (n1 = str1.find(','))) return false;
			if (str2.npos == (n2 = str2.find(','))) return false;

			ImVec2 _top_point = { (float)atof(str1.substr(0, n1).c_str()), (float)atof(str1.substr(n1 + 1, str1.length()).c_str()) };
			ImVec2 _bottom_point = { (float)atof(str2.substr(0, n2).c_str()), (float)atof(str2.substr(n2 + 1, str2.length()).c_str()) };
			ft_slider_random_point_vec.emplace_back(_top_point, _bottom_point);
		}
#endif

		//计算周长
		auto it = ft_slider_random_point_vec.begin();
		for (; it != (ft_slider_random_point_vec.end() - 1); ++it)
		{
			_random_all_length += qu1et_one_quadrangle_length(it->top_point, it->bottom_point, (it + 1)->bottom_point, (it + 1)->top_point);
		}
		//printf("************length:%f****************\n", _random_all_length);
		return true;
	}

	void ft_slider::draw()
	{
		ft_base::draw();
		int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id;
		vres_txt_cd& ptext_cd = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;

		if (_slider_pt._texture_bg_index >= ptext_cd.size())
		{
			printf("invalid texture index:%d\n", _slider_pt._texture_bg_index);
			_slider_pt._texture_bg_index = 0;
		}
		int texture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
		int texture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;
		float sizew = _slider_pt._bg_texture_size.x;
		float sizeh = _slider_pt._bg_texture_size.y;
		ImVec2 abpos = absolute_coordinate_of_base_pos();
		ImVec2 winpos = ImGui::GetWindowPos();
		ImVec2 pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
		ImVec2 pos2 = { pos1.x, pos1.y + sizeh };
		ImVec2 pos3 = { pos1.x + sizew, pos1.y + sizeh };
		ImVec2 pos4 = { pos1.x + sizew, pos1.y };

		ImVec2 uv0 = ImVec2(ptext_cd[_slider_pt._texture_bg_index]._x0 / texture_width, ptext_cd[_slider_pt._texture_bg_index]._y0 / texture_height);
		ImVec2 uv1 = ImVec2(ptext_cd[_slider_pt._texture_bg_index]._x0 / texture_width, (ptext_cd[_slider_pt._texture_bg_index]._y1) / texture_height);
		ImVec2 uv2 = ImVec2((ptext_cd[_slider_pt._texture_bg_index]._x1) / texture_width, (ptext_cd[_slider_pt._texture_bg_index]._y1) / texture_height);
		ImVec2 uv3 = ImVec2((ptext_cd[_slider_pt._texture_bg_index]._x1) / texture_width, (ptext_cd[_slider_pt._texture_bg_index]._y0) / texture_height);

		float offsetx = abpos.x - base_pos().x;
		float offsety = abpos.y - base_pos().y;
		ImVec2 axisBasePos = { offsetx + _slider_pt._bg_axi_pos.x + winpos.x, offsety + _slider_pt._bg_axi_pos.y + winpos.y };
		if (_slider_pt._bg_angle != 0.f)
		{
			pos1 = rotate_point_by_zaxis(pos1, _slider_pt._bg_angle, axisBasePos);
			pos2 = rotate_point_by_zaxis(pos2, _slider_pt._bg_angle, axisBasePos);
			pos3 = rotate_point_by_zaxis(pos3, _slider_pt._bg_angle, axisBasePos);
			pos4 = rotate_point_by_zaxis(pos4, _slider_pt._bg_angle, axisBasePos);
		}
		ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);

		/***********************************************************progress*********************************************************/
		if (_slider_pt._texture_head_index >= ptext_cd.size())
		{
			printf("invalid texture index:%d\n", _slider_pt._texture_head_index);
			_slider_pt._texture_head_index = 0;
		}
		texture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
		texture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;

		abpos = absolute_coordinate_of_base_pos();
		winpos = ImGui::GetWindowPos();

		if (2 == _slider_pt._direction_item) //任意轨道
		{
			if (0 == _random_all_length) return; //第一次进入random时判断
			float _tmp_length = 0.f, _tmp_distance = 0.f;
			int _n_tmp_count = 0;

			auto it = ft_slider_random_point_vec.begin();
			//初始坐标
			_pre_point_2vec2 = *it;
			_next_point_2vec2 = *(it + 1);

			float _tmp_for = 0.f;
			for (int i = 0; it != (ft_slider_random_point_vec.end() - 1); ++it, ++i)
			{
				_tmp_for += qu1et_one_quadrangle_length(it->top_point, it->bottom_point, (it + 1)->bottom_point, (it + 1)->top_point);
				if (_tmp_for > _random_all_length*_slider_pt._position)
				{
					_next_point_2vec2 = *(it + 1);
					_pre_point_2vec2 = *it;
					break;
				}
				else
				{
					_tmp_length = _tmp_for;
					++_n_tmp_count;
				}
			}

			//四边形 上\下\中 边长
			float _top_distance = 0.f, _bottom_distance = 0.f, _mid_distance = 0.f;
			_top_distance = qu1et_thr_length((_pre_point_2vec2.top_point.x - _next_point_2vec2.top_point.x), (_pre_point_2vec2.top_point.y - _next_point_2vec2.top_point.y));
			_bottom_distance = qu1et_thr_length((_pre_point_2vec2.bottom_point.x - _next_point_2vec2.bottom_point.x), (_pre_point_2vec2.bottom_point.y - _next_point_2vec2.bottom_point.y));
			_mid_distance = (_top_distance + _bottom_distance) / 2.f;

			//参与运算的上下长度差
			float _top_l_1 = 0.f, _bottom_l_1 = 0.f;
			_top_l_1 = (_random_all_length*_slider_pt._position - _tmp_length)*_top_distance / _mid_distance;
			_bottom_l_1 = (_random_all_length*_slider_pt._position - _tmp_length)*_bottom_distance / _mid_distance;

			//上坐标()
			_current_point_2vec2_thumb_use.top_point.x = _top_l_1 * (_next_point_2vec2.top_point.x - _pre_point_2vec2.top_point.x) / _top_distance + _pre_point_2vec2.top_point.x;
			_current_point_2vec2_thumb_use.top_point.y = _top_l_1 * (_next_point_2vec2.top_point.y - _pre_point_2vec2.top_point.y) / _top_distance + _pre_point_2vec2.top_point.y;

			//下坐标()
			_current_point_2vec2_thumb_use.bottom_point.x = _bottom_l_1 * (_next_point_2vec2.bottom_point.x - _pre_point_2vec2.bottom_point.x) / _bottom_distance + _pre_point_2vec2.bottom_point.x;
			_current_point_2vec2_thumb_use.bottom_point.y = _bottom_l_1 * (_next_point_2vec2.bottom_point.y - _pre_point_2vec2.bottom_point.y) / _bottom_distance + _pre_point_2vec2.bottom_point.y;


			float tmp_float = 0.f;
			for (auto it = ft_slider_random_point_vec.begin(); it != ft_slider_random_point_vec.end() - 1; ++it) //画进度
			{
				//将此点作为原点进行计算
				ImVec2 uv = ImVec2((ptext_cd[_slider_pt._texture_head_index]._x0) / texture_width, (ptext_cd[_slider_pt._texture_head_index]._y0) / texture_height);
				/*
				* @brief 先计算出顶点数据，从外部导入的顶点数据是单个图片的纹理坐标位置的位置。
				*		 计算纹理坐标是相对于原始的单个图片左上角第一个点来计算
				*
				*/
				if ((tmp_float += qu1et_one_quadrangle_length(it->top_point, it->bottom_point, (it + 1)->bottom_point, (it + 1)->top_point)) > _random_all_length*_slider_pt._position)
				{
					//自身坐标
					pos1 = { abpos.x + winpos.x + it->top_point.x, abpos.y + winpos.y + it->top_point.y };
					pos2 = { abpos.x + winpos.x + it->bottom_point.x, abpos.y + winpos.y + it->bottom_point.y };
					pos3 = { abpos.x + winpos.x + _current_point_2vec2_thumb_use.bottom_point.x, abpos.y + winpos.y + _current_point_2vec2_thumb_use.bottom_point.y };
					pos4 = { abpos.x + winpos.x + _current_point_2vec2_thumb_use.top_point.x, abpos.y + winpos.y + _current_point_2vec2_thumb_use.top_point.y };

					//依靠父节点的坐标和角度计算
					if (_slider_pt._bg_angle != 0.f)
					{
						pos1 = rotate_point_by_zaxis(pos1, _slider_pt._bg_angle, axisBasePos);
						pos2 = rotate_point_by_zaxis(pos2, _slider_pt._bg_angle, axisBasePos);
						pos3 = rotate_point_by_zaxis(pos3, _slider_pt._bg_angle, axisBasePos);
						pos4 = rotate_point_by_zaxis(pos4, _slider_pt._bg_angle, axisBasePos);
					}

					//加上自身的偏移量
					pos1 += _slider_pt._head_pos;
					pos2 += _slider_pt._head_pos;
					pos3 += _slider_pt._head_pos;
					pos4 += _slider_pt._head_pos;

					uv0 = ImVec2(uv.x + it->top_point.x / texture_width, uv.y + it->top_point.y / texture_height);
					uv1 = ImVec2(uv.x + it->bottom_point.x / texture_width, uv.y + it->bottom_point.y / texture_height);
					uv2 = ImVec2(uv.x + _current_point_2vec2_thumb_use.bottom_point.x / texture_width, uv.y + _current_point_2vec2_thumb_use.bottom_point.y / texture_height);
					uv3 = ImVec2(uv.x + _current_point_2vec2_thumb_use.top_point.x / texture_width, uv.y + _current_point_2vec2_thumb_use.top_point.y / texture_height);

					ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);
					break;
				}
				else
				{
					//自身坐标
					pos1 = { abpos.x + winpos.x + it->top_point.x, abpos.y + winpos.y + it->top_point.y };
					pos2 = { abpos.x + winpos.x + it->bottom_point.x, abpos.y + winpos.y + it->bottom_point.y };
					pos3 = { abpos.x + winpos.x + (it + 1)->bottom_point.x, abpos.y + winpos.y + (it + 1)->bottom_point.y };
					pos4 = { abpos.x + winpos.x + (it + 1)->top_point.x, abpos.y + winpos.y + (it + 1)->top_point.y };

					//依靠父节点的坐标和角度计算
					if (_slider_pt._bg_angle != 0.f)
					{
						pos1 = rotate_point_by_zaxis(pos1, _slider_pt._bg_angle, axisBasePos);
						pos2 = rotate_point_by_zaxis(pos2, _slider_pt._bg_angle, axisBasePos);
						pos3 = rotate_point_by_zaxis(pos3, _slider_pt._bg_angle, axisBasePos);
						pos4 = rotate_point_by_zaxis(pos4, _slider_pt._bg_angle, axisBasePos);
					}

					//加上自身的偏移量
					pos1 += _slider_pt._head_pos;
					pos2 += _slider_pt._head_pos;
					pos3 += _slider_pt._head_pos;
					pos4 += _slider_pt._head_pos;

					uv0 = ImVec2(uv.x + it->top_point.x / texture_width, uv.y + it->top_point.y / texture_height);
					uv1 = ImVec2(uv.x + it->bottom_point.x / texture_width, uv.y + it->bottom_point.y / texture_height);
					uv2 = ImVec2(uv.x + (it + 1)->bottom_point.x / texture_width, uv.y + (it + 1)->bottom_point.y / texture_height);
					uv3 = ImVec2(uv.x + (it + 1)->top_point.x / texture_width, uv.y + (it + 1)->top_point.y / texture_height);

					ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);
				}
			}
		}
		else
		{
			if (0 == _slider_pt._direction_item)
			{
				sizew = _slider_pt._head_texture_size.x *_slider_pt._position;
				sizeh = _slider_pt._head_texture_size.y;

				pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
				pos2 = { pos1.x, pos1.y + sizeh };
				pos3 = { pos1.x + sizew, pos1.y + sizeh };
				pos4 = { pos1.x + sizew, pos1.y };
			}
			else if (1 == _slider_pt._direction_item)
			{
				sizew = _slider_pt._head_texture_size.x;
				sizeh = _slider_pt._head_texture_size.y *_slider_pt._position;

				pos1 = { abpos.x + winpos.x, abpos.y + winpos.y - sizeh };
				pos2 = { pos1.x, abpos.y + winpos.y };
				pos3 = { pos1.x + sizew, abpos.y + winpos.y };
				pos4 = { pos1.x + sizew, abpos.y + winpos.y - sizeh };
			}

			uv0 = ImVec2(ptext_cd[_slider_pt._texture_head_index]._x0 / texture_width, ptext_cd[_slider_pt._texture_head_index]._y0 / texture_height);
			uv1 = ImVec2(ptext_cd[_slider_pt._texture_head_index]._x0 / texture_width, (ptext_cd[_slider_pt._texture_head_index]._y1) / texture_height);
			uv2 = ImVec2((ptext_cd[_slider_pt._texture_head_index]._x1) / texture_width, (ptext_cd[_slider_pt._texture_head_index]._y1) / texture_height);
			uv3 = ImVec2((ptext_cd[_slider_pt._texture_head_index]._x1) / texture_width, (ptext_cd[_slider_pt._texture_head_index]._y0) / texture_height);

			if (0 == _slider_pt._direction_item)
			{
				uv2.x = uv1.x + _slider_pt._position*(uv2.x - uv1.x);
				uv3.x = uv0.x + _slider_pt._position*(uv3.x - uv0.x);
			}
			else if (1 == _slider_pt._direction_item)
			{
				uv0.y = uv1.y - _slider_pt._position*(uv1.y - uv0.y);
				uv3.y = uv2.y - _slider_pt._position*(uv2.y - uv3.y);
			}

			offsetx = abpos.x - base_pos().x;
			offsety = abpos.y - base_pos().y;
			axisBasePos = { offsetx + _slider_pt._bg_axi_pos.x + winpos.x, offsety + _slider_pt._bg_axi_pos.y + winpos.y };
			pos1 = rotate_point_by_zaxis(pos1, 0.f, axisBasePos);
			pos2 = rotate_point_by_zaxis(pos2, 0.f, axisBasePos);
			pos3 = rotate_point_by_zaxis(pos3, 0.f, axisBasePos);
			pos4 = rotate_point_by_zaxis(pos4, 0.f, axisBasePos);

			pos1 += _slider_pt._head_pos;
			pos2 += _slider_pt._head_pos;
			pos3 += _slider_pt._head_pos;
			pos4 += _slider_pt._head_pos;

			ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);
		}
		/***********************************************************thumb*********************************************************/
		//thumb
		if (!_slider_pt._thumb_visible) return;
		if (2 == _slider_pt._direction_item) //任意轨道时，图标移动计算
		{
			/*
			 * @brief 自定义轨道时游标的位置，目前做法不满足一般需求，需改变。
			 * 目前做法：确定当前位置的中心点后，依靠图片的长和宽算出对应的定点坐标
			 * 导致的问题，图片不能旋转。
			 * 后续做法再确定
			 */

			ImVec2 _center_positon_point = ImVec2((_current_point_2vec2_thumb_use.top_point.x + _current_point_2vec2_thumb_use.bottom_point.x) / 2, (_current_point_2vec2_thumb_use.top_point.y + _current_point_2vec2_thumb_use.bottom_point.y) / 2);

			ImVec2 uv0 = ImVec2(ptext_cd[_slider_pt._texture_thumb_index]._x0 / texture_width, ptext_cd[_slider_pt._texture_thumb_index]._y0 / texture_height);
			ImVec2 uv1 = ImVec2(ptext_cd[_slider_pt._texture_thumb_index]._x0 / texture_width, (ptext_cd[_slider_pt._texture_thumb_index]._y1) / texture_height);
			ImVec2 uv2 = ImVec2((ptext_cd[_slider_pt._texture_thumb_index]._x1) / texture_width, (ptext_cd[_slider_pt._texture_thumb_index]._y1) / texture_height);
			ImVec2 uv3 = ImVec2((ptext_cd[_slider_pt._texture_thumb_index]._x1) / texture_width, (ptext_cd[_slider_pt._texture_thumb_index]._y0) / texture_height);

			//窗口坐标加自身坐标
			pos1 = ImVec2(abpos.x + winpos.x + _center_positon_point.x - _slider_pt._thumb_texture_size.x / 2, abpos.y + winpos.y + _center_positon_point.y - _slider_pt._thumb_texture_size.y / 2);
			pos2 = ImVec2(abpos.x + winpos.x + _center_positon_point.x - _slider_pt._thumb_texture_size.x / 2, abpos.y + winpos.y + _center_positon_point.y + _slider_pt._thumb_texture_size.y / 2);
			pos3 = ImVec2(abpos.x + winpos.x + _center_positon_point.x + _slider_pt._thumb_texture_size.x / 2, abpos.y + winpos.y + _center_positon_point.y + _slider_pt._thumb_texture_size.y / 2);
			pos4 = ImVec2(abpos.x + winpos.x + _center_positon_point.x + _slider_pt._thumb_texture_size.x / 2, abpos.y + winpos.y + _center_positon_point.y - _slider_pt._thumb_texture_size.y / 2);

			//依靠父节点的坐标和角度计算
			if (_slider_pt._bg_angle != 0.f)
			{
				pos1 = rotate_point_by_zaxis(pos1, _slider_pt._bg_angle, axisBasePos);
				pos2 = rotate_point_by_zaxis(pos2, _slider_pt._bg_angle, axisBasePos);
				pos3 = rotate_point_by_zaxis(pos3, _slider_pt._bg_angle, axisBasePos);
				pos4 = rotate_point_by_zaxis(pos4, _slider_pt._bg_angle, axisBasePos);
			}

			pos1 += _slider_pt._thumb_pos;
			pos2 += _slider_pt._thumb_pos;
			pos3 += _slider_pt._thumb_pos;
			pos4 += _slider_pt._thumb_pos;

			//计算当前轨道的角度后旋转四个点 _next_point_2vec2.top  _pre_point_2vec2.top
			//注释：由于角度计算是根据采集的点进行的，所以受采集点影响比较大，要做到旋转很难
			//
			//float _ang = atan((_next_point_2vec2.top_point.y - _pre_point_2vec2.top_point.y) / (_next_point_2vec2.top_point.x - _pre_point_2vec2.top_point.x));
			//ImVec2 _center_tmp(_center_positon_point.x +abpos.x + winpos.x, _center_positon_point.y +abpos.y + winpos.y); //中心点需加窗口坐标
			//pos1 = rotate_point_by_zaxis(pos1, _ang, _center_tmp);
			//pos2 = rotate_point_by_zaxis(pos2, _ang, _center_tmp);
			//pos3 = rotate_point_by_zaxis(pos3, _ang, _center_tmp);
			//pos4 = rotate_point_by_zaxis(pos4, _ang, _center_tmp);

			//printf("angle:%f\n", _ang);
			//printf("_center_positon_point:%f,%f\n", _center_tmp.x, _center_tmp.y);
			//printf("pos1:%f,%f\n", pos1.x, pos1.y);
			//printf("pos2:%f,%f\n", pos2.x, pos2.y);
			//printf("pos3:%f,%f\n", pos3.x, pos3.y);
			//printf("pos4:%f,%f\n", pos4.x, pos4.y);

			ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);
		}
		else
		{
			if (_slider_pt._texture_thumb_index >= ptext_cd.size())
			{
				printf("invalid texture index:%d\n", _slider_pt._texture_thumb_index);
				_slider_pt._texture_thumb_index = 0;
			}
			texture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
			texture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;

			abpos = absolute_coordinate_of_base_pos();
			winpos = ImGui::GetWindowPos();

			pos1 = { abpos.x + winpos.x, abpos.y + winpos.y };
			pos2 = { pos1.x, pos1.y + _slider_pt._thumb_texture_size.y };
			pos3 = { pos1.x + _slider_pt._thumb_texture_size.x, pos1.y + _slider_pt._thumb_texture_size.y };
			pos4 = { pos1.x + _slider_pt._thumb_texture_size.x, pos1.y };

			uv0 = ImVec2(ptext_cd[_slider_pt._texture_thumb_index]._x0 / texture_width, ptext_cd[_slider_pt._texture_thumb_index]._y0 / texture_height);
			uv1 = ImVec2(ptext_cd[_slider_pt._texture_thumb_index]._x0 / texture_width, (ptext_cd[_slider_pt._texture_thumb_index]._y1) / texture_height);
			uv2 = ImVec2((ptext_cd[_slider_pt._texture_thumb_index]._x1) / texture_width, (ptext_cd[_slider_pt._texture_thumb_index]._y1) / texture_height);
			uv3 = ImVec2((ptext_cd[_slider_pt._texture_thumb_index]._x1) / texture_width, (ptext_cd[_slider_pt._texture_thumb_index]._y0) / texture_height);

			offsetx = abpos.x - base_pos().x;
			offsety = abpos.y - base_pos().y;
			axisBasePos = { offsetx + _slider_pt._bg_axi_pos.x + winpos.x, offsety + _slider_pt._bg_axi_pos.y + winpos.y };

			pos1 += _slider_pt._thumb_pos;
			pos2 += _slider_pt._thumb_pos;
			pos3 += _slider_pt._thumb_pos;
			pos4 += _slider_pt._thumb_pos;
			if (0 == _slider_pt._direction_item)
			{
				pos1.x += _slider_pt._head_texture_size.x*_slider_pt._position;
				pos2.x += _slider_pt._head_texture_size.x*_slider_pt._position;
				pos3.x += _slider_pt._head_texture_size.x*_slider_pt._position;
				pos4.x += _slider_pt._head_texture_size.x*_slider_pt._position;
			}
			else if (1 == _slider_pt._direction_item)
			{
				pos1.y -= _slider_pt._head_texture_size.y*_slider_pt._position;
				pos2.y -= _slider_pt._head_texture_size.y*_slider_pt._position;
				pos3.y -= _slider_pt._head_texture_size.y*_slider_pt._position;
				pos4.y -= _slider_pt._head_texture_size.y*_slider_pt._position;
			}

			pos1 = rotate_point_by_zaxis(pos1, 0.f, axisBasePos);
			pos2 = rotate_point_by_zaxis(pos2, 0.f, axisBasePos);
			pos3 = rotate_point_by_zaxis(pos3, 0.f, axisBasePos);
			pos4 = rotate_point_by_zaxis(pos4, 0.f, axisBasePos);

			ImGui::ImageQuad((ImTextureID)texture_id, pos1, pos2, pos3, pos4, uv0, uv1, uv2, uv3);
		}
	}
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
	static void ShowHelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
	static bool get_texture_item(void* data, int idx, const char** out_str)
	{
		*out_str = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates[idx]._file_name.c_str();
		return true;
	}
	void ft_slider::draw_peroperty_page(int property_part)
	{
		ft_base::draw_peroperty_page();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Text("progress value:");
		ImGui::SameLine(); ShowHelpMarker("values of the progress(0.0 ~ 1.0)!\n");
		ImGui::SliderFloat("value", &_slider_pt._position, 0.f, 1.f);
		ImGui::Text("slider direction:");
		ImGui::Combo("direction", &_slider_pt._direction_item, direction_iitem, ARRAY_COUNT_DIRECTION_ITEM);
		ImGui::SameLine(); ShowHelpMarker("x turn right, y turn top, radom don't finish!\n");
		if (2 == _slider_pt._direction_item)
		{
			ImGui::Text("import file:");
			ImGui::InputText("", _slider_pt._cbuffer_random_text, sizeof(_slider_pt._cbuffer_random_text));
			ImGui::SameLine();
			if (ImGui::Button("import"))
			{
				if (NULL == _slider_pt._cbuffer_random_text) return;
				if (read_point_position_file(_slider_pt._cbuffer_random_text))
				{
					MessageBox(GetForegroundWindow(), _slider_pt._cbuffer_random_text, "read point file success", MB_OK);
				}
			}
			ImGui::SameLine();
			ShowHelpMarker("file must from file list, so we must load file before!\n");
		}
		ImGui::Text("axi pos:");
		ImGui::SliderFloat("ax", &_slider_pt._bg_axi_pos.x, 1.f, base_ui_component::screenw);
		ImGui::SliderFloat("ay", &_slider_pt._bg_axi_pos.y, 1.f, base_ui_component::screenh);
		ImGui::Text("angle:");
		ImGui::SliderFloat("a", &_slider_pt._bg_angle, 0.f, 1.f);
		ImGui::Text("bg image:");

		auto& res_coors = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
		int isize = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates.size();
		ImGui::Combo("texture index:", &_slider_pt._texture_bg_index, &get_texture_item, &g_vres_texture_list[g_cur_texture_id_index], isize);
		ImGui::SameLine(); ShowHelpMarker("select a image from image resource!\n");
		ImGui::Spacing();
		float reswidth = res_coors[_slider_pt._texture_bg_index].owidth();
		float resheight = res_coors[_slider_pt._texture_bg_index].oheight();
		ImGui::Text("original size:%f,%f", reswidth, resheight);
		_slider_pt._bg_texture_size.x = reswidth;
		_slider_pt._bg_texture_size.y = resheight;
		ImGui::Spacing();
		if (reswidth > 0)
		{
			float draw_height = imge_edit_view_width*resheight / reswidth;
			ImVec2 draw_size(imge_edit_view_width, draw_height);
			int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id;
			float wtexture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
			float wtexture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;

			ImVec2 uv0(res_coors[_slider_pt._texture_bg_index]._x0 / wtexture_width, res_coors[_slider_pt._texture_bg_index]._y0 / wtexture_height);
			ImVec2 uv1(res_coors[_slider_pt._texture_bg_index]._x1 / wtexture_width, res_coors[_slider_pt._texture_bg_index]._y1 / wtexture_height);
			ImGui::Image((ImTextureID)texture_id, draw_size, uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
		}
		/***************************************************head*************************************************************/
		ImGui::Text("head pos:");
		ImGui::SliderFloat("head w", &_slider_pt._head_pos.x, 0.f, base_ui_component::screenw);
		ImGui::SliderFloat("head h", &_slider_pt._head_pos.y, 0.f, base_ui_component::screenh);
		ImGui::Text("head image:");
		auto& res_coors1 = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
		ImGui::Combo("texture index1:", &_slider_pt._texture_head_index, &get_texture_item, &g_vres_texture_list[g_cur_texture_id_index], isize);
		ImGui::SameLine(); ShowHelpMarker("select a image from image resource!\n");
		ImGui::Spacing();
		float reswidth1 = res_coors1[_slider_pt._texture_head_index].owidth();
		float resheight1 = res_coors1[_slider_pt._texture_head_index].oheight();
		ImGui::Text("original size:%f,%f", reswidth1, resheight1);
		_slider_pt._head_texture_size.x = reswidth1;
		_slider_pt._head_texture_size.y = resheight1;
		ImGui::Spacing();
		if (reswidth1 > 0)
		{
			float draw_height = imge_edit_view_width*resheight1 / reswidth1;
			ImVec2 draw_size(imge_edit_view_width, draw_height);
			int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id;
			float wtexture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
			float wtexture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;

			ImVec2 uv0(res_coors1[_slider_pt._texture_head_index]._x0 / wtexture_width, res_coors1[_slider_pt._texture_head_index]._y0 / wtexture_height);
			ImVec2 uv1(res_coors1[_slider_pt._texture_head_index]._x1 / wtexture_width, res_coors1[_slider_pt._texture_head_index]._y1 / wtexture_height);
			ImGui::Image((ImTextureID)texture_id, draw_size, uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
		}
		/*****************************************************thumb***********************************************************/
		ImGui::Checkbox("thumb visible", &_slider_pt._thumb_visible);
		if (_slider_pt._thumb_visible)
		{
			ImGui::Text("thumb pos:");
			ImGui::SliderFloat("thumb w", &_slider_pt._thumb_pos.x, 0.f, base_ui_component::screenw);
			ImGui::SliderFloat("thumb h", &_slider_pt._thumb_pos.y, 0.f, base_ui_component::screenh);
			ImGui::Text("thumb image:");
			auto& res_coors2 = g_vres_texture_list[g_cur_texture_id_index].vtexture_coordinates;
			ImGui::Combo("texture index2:", &_slider_pt._texture_thumb_index, &get_texture_item, &g_vres_texture_list[g_cur_texture_id_index], isize);
			ImGui::SameLine(); ShowHelpMarker("select a image from image resource!\n");
			ImGui::Spacing();
			float reswidth2 = res_coors2[_slider_pt._texture_thumb_index].owidth();
			float resheight2 = res_coors2[_slider_pt._texture_thumb_index].oheight();
			ImGui::Text("original size:%f,%f", reswidth2, resheight2);
			_slider_pt._thumb_texture_size.x = reswidth2;
			_slider_pt._thumb_texture_size.y = resheight2;
			ImGui::Spacing();
			if (reswidth2 > 0)
			{
				float draw_height = imge_edit_view_width*resheight2 / reswidth2;
				ImVec2 draw_size(imge_edit_view_width, draw_height);
				int texture_id = g_vres_texture_list[g_cur_texture_id_index].texture_id;
				float wtexture_width = g_vres_texture_list[g_cur_texture_id_index].texture_width;
				float wtexture_height = g_vres_texture_list[g_cur_texture_id_index].texture_height;

				ImVec2 uv0(res_coors2[_slider_pt._texture_thumb_index]._x0 / wtexture_width, res_coors2[_slider_pt._texture_thumb_index]._y0 / wtexture_height);
				ImVec2 uv1(res_coors2[_slider_pt._texture_thumb_index]._x1 / wtexture_width, res_coors2[_slider_pt._texture_thumb_index]._y1 / wtexture_height);
				ImGui::Image((ImTextureID)texture_id, draw_size, uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
			}
		}
	}

	bool ft_slider::init_from_json(Value& jvalue)
	{
		ft_base::init_from_json(jvalue);

		_slider_pt._position = jvalue["progress_value"].asDouble();
		_slider_pt._direction_item = jvalue["direction"].asInt();
		strcpy(_slider_pt._cbuffer_random_text, jvalue["random_point_file"].asString().c_str());

		Value& bg = jvalue["bg_pos"];
		_slider_pt._bg_axi_pos.y = bg["bg_pos_h"].asDouble();
		_slider_pt._bg_axi_pos.x = bg["bg_pos_w"].asDouble();
		_slider_pt._texture_bg_index = bg["bg_texture_index"].asInt();
		_slider_pt._bg_angle = bg["bg_angle"].asDouble();

		Value& head = jvalue["head_pos"];
		_slider_pt._head_pos.y = head["head_pos_h"].asDouble();
		_slider_pt._head_pos.x = head["head_pos_w"].asDouble();
		_slider_pt._texture_head_index = head["head_texture_index"].asInt();

		Value& thumb = jvalue["thumb_pos"];
		_slider_pt._thumb_pos.y = thumb["thumb_pos_h"].asDouble();
		_slider_pt._thumb_pos.x = thumb["thumb_pos_w"].asDouble();
		_slider_pt._texture_thumb_index = thumb["thumb_texture_index"].asInt();
		_slider_pt._thumb_visible = thumb["thumb_visible"].asBool();

		Value& slidersize = jvalue["slider_size"];
		_slider_pt._bg_texture_size.x = slidersize["bg_texture_size_w"].asDouble();
		_slider_pt._bg_texture_size.y = slidersize["bg_texture_size_h"].asDouble();

		_slider_pt._head_texture_size.x = slidersize["head_texture_size_w"].asDouble();
		_slider_pt._head_texture_size.y = slidersize["head_texture_size_h"].asDouble();

		_slider_pt._thumb_texture_size.x = slidersize["thumb_texture_size_w"].asDouble();
		_slider_pt._thumb_texture_size.y = slidersize["thumb_texture_size_h"].asDouble();

		if (2 == _slider_pt._direction_item) //如果保存的是random，这时候就需从文件中读出点数据
		{
			printf("filename:%s\n", _slider_pt._cbuffer_random_text);
			if (NULL == _slider_pt._cbuffer_random_text) return true;
			read_point_position_file(_slider_pt._cbuffer_random_text);
		}

		return true;
	}

	bool ft_slider::init_json_unit(Value& junit)
	{
		ft_base::init_json_unit(junit);
		junit["progress_value"] = _slider_pt._position;
		junit["direction"] = _slider_pt._direction_item;
		junit["random_point_file"] = _slider_pt._cbuffer_random_text;

		Value bg(objectValue);
		bg["bg_pos_w"] = _slider_pt._bg_axi_pos.x;
		bg["bg_pos_h"] = _slider_pt._bg_axi_pos.y;
		bg["bg_texture_index"] = _slider_pt._texture_bg_index;
		bg["bg_angle"] = _slider_pt._bg_angle;
		junit["bg_pos"] = bg;

		Value head(objectValue);
		head["head_pos_w"] = _slider_pt._head_pos.x;
		head["head_pos_h"] = _slider_pt._head_pos.y;
		head["head_texture_index"] = _slider_pt._texture_head_index;
		junit["head_pos"] = head;

		Value thumb(objectValue);
		thumb["thumb_pos_w"] = _slider_pt._thumb_pos.x;
		thumb["thumb_pos_h"] = _slider_pt._thumb_pos.y;
		thumb["thumb_texture_index"] = _slider_pt._texture_thumb_index;
		thumb["thumb_visible"] = _slider_pt._thumb_visible;
		junit["thumb_pos"] = thumb;

		Value slidersize(objectValue);
		slidersize["bg_texture_size_w"] = _slider_pt._bg_texture_size.x;
		slidersize["bg_texture_size_h"] = _slider_pt._bg_texture_size.y;

		slidersize["head_texture_size_w"] = _slider_pt._head_texture_size.x;
		slidersize["head_texture_size_h"] = _slider_pt._head_texture_size.y;

		slidersize["thumb_texture_size_w"] = _slider_pt._thumb_texture_size.x;
		slidersize["thumb_texture_size_h"] = _slider_pt._thumb_texture_size.y;
		junit["slider_size"] = slidersize;

		return true;
	}
#endif

	
}