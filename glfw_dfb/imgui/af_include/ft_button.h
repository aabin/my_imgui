#pragma once
#include "ft_base.h"
namespace auto_future
{
	class AFG_EXPORT ft_button :
		public ft_base
	{
	public:
		enum state
		{
			en_normal,
			en_pressed,
			en_selected,
			en_state_cnt
		};
	private:
		DEF_STRUCT_WITH_INIT(intl_pt, _pt,
			(int, _image_normal_txt, {0}),
			(int, _image_press_txt, { 0 }), 
			(int, _image_select_txt, { 0 }))
		state _state = { en_normal };

	public:

		ft_button();
		//ft_button(int tid) :base_ui_component(tid){}
		void draw();
	
		void set_state(state st)
		{
			_state = st;
		}
		state get_state()
		{
			return _state;
		}

		int get_cur_txt_id()
		{
			switch (_state)
			{
			case auto_future::ft_button::en_normal:
				return _pt._image_normal_txt;
			case auto_future::ft_button::en_pressed:
				return _pt._image_press_txt;
			case auto_future::ft_button::en_selected:
				return _pt._image_select_txt;
			}
			return 0;
		}


		bool handle_mouse();
	};
	REGISTER_CONTROL(ft_button)
}