#pragma once
#include "ft_base.h"
#include "af_shader.h"
#include "af_model.h"
namespace auto_future
{

	class AFG_EXPORT ft_modeling_3d :
		public ft_base
	{

		DEF_STRUCT_WITH_INIT(pty_page, _pty_page,
			(char, _model_name[FILE_NAME_LEN]),
			(camera, _cam),
			(projection, _pj),
			(transformation, _trans),//directional_light,point_light
			(af_vec3, _light_ambient_clr),
			(af_vec3, _light_diffuse_clr),
			(af_vec3, _light_specular_clr),
			(af_vec3,_light_position_shd),
			(float, _light_constant,{1.f}),
			(float, _light_linear,{0.09f}),
			(float, _light_quadratic, { 0.032f })
			)
		shared_ptr<af_shader> _pshd_modeling;
		shared_ptr<af_model> _pmodel;
	public:
		ft_modeling_3d();
		~ft_modeling_3d();
		void draw();
		void link()
		{
			auto imodel = g_mmodel_list.find(_pty_page._model_name);
			if (imodel != g_mmodel_list.end())
			{
				_pmodel = imodel->second;
			}
		}
	};
	REGISTER_CONTROL(ft_modeling_3d)
}