
#include "mobileye_adas.h"
#include "msg_host_n.h"
#include "af_bind.h"
#include "af_primitive_object.h"
#include "af_material.h"
#include "enum_txt_name0.h"
#include "afg.h"
//#include "coordinate_change.h"
using namespace auto_future_utilities;
using namespace auto_future;

extern u8 navi_direct_state;
extern msg_host_n g_msg_host;
extern bool be_turn_left;
extern bool be_turn_right;
const char* road_primitice = "road.FBX0";
const char* mtl_name = "mt_sp";
//DefStructCoordinateChange g_from_xie;
//eye_2_mobile_eyex=0.36m
//eye_2_mobile_eyey=0.0658m
//eye_2_mobile_eyez=-0.538m
enum en_lane_state
{
	en_lane_normal,
	en_lane_ldw_left,
	en_lane_ldw_right,
	en_lane_num,
};
u8 navi_state = 0;
const char* lane_state_txt_name[2][en_lane_num]=
{
	{"road.png","ldw_left.png","ldw_right.png"},
	{"road_navi.png", "ldw_left_navi.png", "ldw_right_navi.png" },
};
const char* ldw_left_txt = "ldw_left.png";
const char* ldw_right_txt = "ldw_right.png";
const char* road_txt = "road.png";
base_ui_component* obstacles_group=nullptr;
static int obstacles_cnt = 0;
MSC_PACK_HEAD
//bool set_mp_text_uf(const char* mtl_nm, const char* txt_uf_nm, const char* txt_nm)
//{
//	const auto& imtl = g_material_list.find(mtl_nm);
//	if (imtl != g_material_list.end())
//	{
//		auto& mtl = *imtl->second;
//		auto& mp_sduf = mtl.get_mp_sd_uf();
//		const auto& isduf = mp_sduf.find(txt_uf_nm);
//		if (isduf != mp_sduf.end())
//		{
//			auto& txt_sduf = *isduf->second;
//			char* data_hd = (char*)txt_sduf.get_data_head();
//			memcpy(data_hd, txt_nm, strlen(txt_nm));
//			data_hd[strlen(txt_nm)] = '\0';
//			txt_sduf.link();
//			return true;
//		}
//	}
//	return false;
//}


//ps_primrive_object ps_prm_road;
/*
const int numb_of_vertex = 126;
const int len_of_verterx = 8;
static float base_road_vertexs[numb_of_vertex][len_of_verterx];
static float road_vertexs[numb_of_vertex][len_of_verterx];
static bool road_veters_left[numb_of_vertex];
static float base_y=0.;

void init_raod_vertex()
{
	for (int ix = 0; ix < numb_of_vertex;ix++)
	{
		auto& vertex_u = base_road_vertexs[ix];
		road_veters_left[ix] = vertex_u[3] < 0;
		if (vertex_u[4]>base_y)
		{
			base_y = vertex_u[4];
		}
	}
}
void calcu_offset_x(float curvature, float lane_heading,int ix)
{
	float yvalue = base_y-base_road_vertexs[ix][4];
	road_vertexs[ix][3] = curvature*yvalue*yvalue + lane_heading*yvalue + base_road_vertexs[ix][3];
}
void calcu_vertex(float curvature_left, float lane_heading_left, float curvature_right, float lane_heading_right)
{
	for (int ix = 0; ix < numb_of_vertex;ix++)
	{
		if (road_veters_left[ix])
		{
			calcu_offset_x(curvature_left, lane_heading_left, ix);
		}
		else
		{
			calcu_offset_x(curvature_right, lane_heading_right, ix);
		}
	}
}
*/
//static const char* mtl_name = "mt_sp";
void print_buff(u8* pbuff, int len)
{
	static char num_tb[0x10] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};
	struct buff_data
	{
		u8 data_l : 4;
		u8 data_h : 4;
	};
	string pt_buff;
	while (len > 0)
	{
		buff_data* pdata = (buff_data*)pbuff++;
		len--;
		pt_buff += " x";
		pt_buff += num_tb[pdata->data_h];
		pt_buff += num_tb[pdata->data_l];
	}
	printf("%s\n", pt_buff.c_str());
}

ps_mtl mtl_lane;
enum lane_type
{
	en_lane_dashed,
	en_lane_solid,
	en_lane_none,
	en_lane_road_edge,
	en_lane_double_lane_mark,
	en_lane_bott_dots,
	en_lane_invalid,
};
bool prohibit_lane_swith(u8 lane_type)
{
	bool ret= lane_type==en_lane_solid||lane_type==en_lane_road_edge||lane_type==en_lane_double_lane_mark||lane_type==en_lane_invalid;
	return ret;
}
u8 left_lane_type=0,right_lane_type=0;
static u16 left_curvature_stm = 0, left_lane_heading_stm = 0;
static u16 right_curvature_stm = 0, right_lane_heading_stm = 0;
static float left_position=-180,right_position=180;

float uf_value = 0.f;
void calcu_left_right_lanes()
{
	//return;
	if (left_curvature_stm != 0 && left_lane_heading_stm != 0 && right_curvature_stm != 0 && right_lane_heading_stm!=0)
	{
		float cuv_lt = (left_curvature_stm - 0x7fff) / 1024000.0;
		float lh_lt = (left_lane_heading_stm - 0x7fff) / 1024.0;
		float cuv_rt = (right_curvature_stm - 0x7fff) / 1024000.0;
		float lh_rt = (right_lane_heading_stm - 0x7fff) / 1024.f;
		float left_base=-left_position*100.f;
		float right_base=-right_position*100.f;
		float of_eye=30.f;
		bool be_cross=cuv_lt>0&&cuv_rt<0;
		bool be_departure=cuv_lt<0&&cuv_rt>0;
		//if (1)//(cuv_lt > 0.0 && cuv_rt > 0.0 || cuv_lt < 0 && cuv_rt < 0)
		{
			printf("cuv_lt=%f,lh_lt=%f,ofs_l=%f,cuv_rt=%f,lh_rt=%f,ofs_r=%f\n", cuv_lt, lh_lt,left_position, cuv_rt, lh_rt,right_position);
			
			mtl_lane->set_value("curv_l", &cuv_lt, 1);
			mtl_lane->set_value("head_l", &lh_lt, 1);
			mtl_lane->set_value("curv_r", &cuv_rt, 1);
			mtl_lane->set_value("head_r", &lh_rt, 1);
			mtl_lane->set_value("left_base", &left_base, 1);
			mtl_lane->set_value("right_base", &right_base, 1);
			mtl_lane->set_value("of_eye", &of_eye, 1);
			//bool right_switch = cuv_lt > 0.00055f;
			//set_property_aliase_value("show_right_bend", &right_switch);
			//bool left_switch = cuv_lt < -0.00055f;
			//set_property_aliase_value("show_left_bend", &left_switch);


			//calcu_vertex(cuv_lt, lh_lt, cuv_rt, lh_rt);
			//printf("left_curvature_stm=0x%x,left_lane_heading_stm=0x%x,right_curvature_stm=0x%x,right_lane_heading_stm=0x%x\n", left_curvature_stm, left_lane_heading_stm, right_curvature_stm, right_lane_heading_stm);
			//printf("cuv_lt=%f,lh_lt=%f,cuv_rt=%f,lh_rt=%f\n", cuv_lt, lh_lt, cuv_rt, lh_rt);
			//glBindBuffer(GL_ARRAY_BUFFER, ps_prm_road->_vbo);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*ps_prm_road->_vertex_buf_len, road_vertexs, GL_DYNAMIC_DRAW);
			//update_prm_vbo(road_primitice, road_vertexs[0], numb_of_vertex * len_of_verterx);


			left_curvature_stm = left_lane_heading_stm = right_curvature_stm = right_lane_heading_stm = 0;
		}
	}
}
void register_adas_cmd_handl()
{
	//const auto& iprm = g_primitive_list.find(road_primitice);
	//if (iprm!=g_primitive_list.end())
	//{
	//	ps_prm_road = iprm->second;
	//	auto& ps_file = ps_prm_road->_ps_file;
	//	char* phead = (char*)ps_file->_pbin;
	//	GLuint* phead_buff_len = (GLuint*)phead;
	//	phead += 4;
	//	assert(*phead_buff_len == numb_of_vertex * len_of_verterx * sizeof(float));
	//	memcpy(&road_vertexs[0][0], phead, *phead_buff_len);
	//	memcpy(&base_road_vertexs[0][0], phead, *phead_buff_len);
	//	init_raod_vertex();
	//}
	/*if (get_prm_data(road_primitice, &road_vertexs[0][0], numb_of_vertex * len_of_verterx))
	{
	memcpy(&base_road_vertexs[0][0], road_vertexs, numb_of_vertex * len_of_verterx * sizeof(float));
	init_raod_vertex();
	}*/
	//Coordinate_Change_Init(&g_from_xie);
	const auto& imtl = g_material_list.find("mt_sp");
	assert(imtl != g_material_list.end());
	mtl_lane = imtl->second;
	/*uf_value = 350.f;
	mtl_lane->set_value("base_y", &uf_value, 1);*/
	bool bevalue = false;
	set_property_aliase_value("show_scene_base", &bevalue);

	obstacles_group = get_aliase_ui_control("show_obstacles");
	g_msg_host.attach_monitor("can message", [&](u8* pbuff, int len){
		//printf("can message:\n");
		//print_buff(pbuff, len);
		pbuff += 2;
		struct GNU_DEF can_msg
		{
			u8 b0_sound_type : 3;
			u8 b0_time_indicator : 2;
			u8 b0_undocumented : 3;

			u8 b1_0 : 1;
			u8 b1_reserved_n : 4;
			u8 b1_zero_spped : 1;
			u8 b1_reserved : 2;

			u8 b2_headway_valid : 1;
			u8 b2_headway_measurement : 7;

			u8 b3_error : 1;
			u8 b3_error_code : 7;

			u8 b4_ldw_off : 1;
			u8 b4_left_ldw_on : 1;
			u8 b4_right_ldw_on : 1;
			u8 b4_fcw_on : 1;
			u8 b4_undocumented : 2;
			u8 b4_maintenance : 1;
			u8 b4_failsafe : 1;

			u8 b5_zero : 1;
			u8 b5_peds_fcw : 1;
			u8 b5_peds_in_dz : 1;
			u8 b5_reserved2 : 2;
			u8 b5_tamper_alert : 1;
			u8 b5_reseved : 1;
			u8 b5_tsr_enabled : 1;

			u8 b6_tsr_warning_level : 3;
			u8 b6_reserved : 5;

			u8 b7_headway_warning_level : 2;
			u8 b7_hw_repeatable_enabled : 1;
			u8 b7_reserved : 5;
		};
		can_msg* pcan_msg = (can_msg*)pbuff;
		bool be_show;
		if (pcan_msg->b2_headway_valid)
		{
			int dist_id = 0;
			static u8 segs[] = { 24, 20, 16, 12, 8, 4 };
			u8 head_way_mm = pcan_msg->b2_headway_measurement;
			for (; dist_id < sizeof(segs); dist_id++)
			{
				if (head_way_mm > segs[dist_id])
				{
					break;
				}
			}
			dist_id--;
			if (dist_id > 0)
			{
				for (int ix = 0; ix < 6; ix++)
				{
					be_show = ix == dist_id;
					char seg_name[50] = "";
					sprintf(seg_name, "show_distance%d", ix);
					set_property_aliase_value(seg_name, &be_show);
				}

			}
		}
		else
		{
			for (int ix = 0; ix < 6; ix++)
			{
				be_show = false;
				char seg_name[50] = "";
				sprintf(seg_name, "show_distance%d", ix);
				set_property_aliase_value(seg_name, &be_show);
			}
		}
		if (navi_direct_state == 0)
		{
			if (pcan_msg->b4_ldw_off)
			{
				set_mp_text_uf("mt_sp", "text", lane_state_txt_name[navi_state][en_lane_normal]);
			}
			else
			{
				if ((pcan_msg->b0_sound_type == 1 || pcan_msg->b4_left_ldw_on)
					&& pcan_msg->b4_right_ldw_on == 0 && !be_turn_left)
				{
					set_mp_text_uf("mt_sp", "text", lane_state_txt_name[navi_state][en_lane_ldw_left]);
				}
				else
				if ((pcan_msg->b0_sound_type == 2 || pcan_msg->b4_right_ldw_on)
					&& pcan_msg->b4_left_ldw_on == 0 && !be_turn_right)
				{
					set_mp_text_uf("mt_sp", "text", lane_state_txt_name[navi_state][en_lane_ldw_right]);
				}
			}
		}


		be_show = pcan_msg->b5_peds_in_dz;
		if (pcan_msg->b5_peds_in_dz)
		{
			be_show = true;
			/*if (pcan_msg->b5_peds_fcw)
			{
			set_property_aliase_value("peds_war2", &be_show);
			}
			else
			{
			set_property_aliase_value("peds_war1", &be_show);
			}*/
		}
		else
		{
			be_show = false;
			/*set_property_aliase_value("peds_war1", &be_show);
			set_property_aliase_value("peds_war2", &be_show);*/
		}
		//set_property_aliase_value("")
	});
	g_msg_host.attach_monitor("car info", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF car_info
		{
			u8 b0_brakes : 1;
			u8 b0_left_signal : 1;
			u8 b0_right_signal : 1;
			u8 b0_wipers : 1;
			u8 b0_low_beam : 1;
			u8 b0_high_beam : 1;
			u8 b0_resvered1 : 1;
			u8 b0_reserved : 1;

			u8 b1_reserved2 : 3;
			u8 b1_wipers_available : 1;
			u8 b1_low_beam_available : 1;
			u8 b1_high_beam_available : 1;
			u8 b1_reserved : 1;
			u8 b1_speed_available : 1;

			u8 b2_speed;
			u8 b3_reserved;
			u8 b4_reserved;
			u8 b5_reserved;
			u8 b6_reserved;
			u8 b7_reserved;
		};
		car_info* pcar_info = (car_info*)pbuff;

		//bool be_show;
		//int txt_idx = 0;
		//if (pcar_info->b0_high_beam||pcar_info->b0_low_beam)
		//{
		//	be_show = true;
		//	txt_idx = pcar_info->b0_high_beam ? en_vinfo_fullbeam_headlight_png : en_vinfo_dipped_headlight_png;
		//	set_property_aliase_value("show_beam_low_high", &be_show);
		//	set_property_aliase_value("control_beam_low_high", &txt_idx);
		//}
		//else{
		//	be_show = false;
		//	set_property_aliase_value("show_beam_low_high", &be_show);
		//}

		//if(pcar_info)
	});
	g_msg_host.attach_monitor("tsr message", [&](u8* pbuff, int len){
		u16* ptsr_id = (u16*)pbuff;
		pbuff += 2;
		struct GNU_DEF tsr_message
		{
			u8 b0_vision_only_sign_type;
			u8 b1_vision_only_supplementary_sign_type;
			u8 b2_sign_position_x;
			u8 b3_sign_position_y : 7;
			u8 b3_na : 1;
			u8 b4_position_z : 6;
			u8 b4_na : 2;
			u8 b5_filter_type;
			u8 b6_na;
			u8 b7_na;
		};
		tsr_message* ptsr_message = (tsr_message*)pbuff;
	});
	g_msg_host.attach_monitor("tsr display", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF tsr_display
		{
			u8 b0_vision_only_sign_type_display1;
			u8 b1_vision_only_supplementary_sign_type_display1;
			u8 b2_vision_only_sign_type_display2;
			u8 b3_vision_only_supplementary_sign_type_display2;
			u8 b4_vision_only_sign_type_display3;
			u8 b5_vision_only_supplementary_sign_type_display3;
			u8 b6_vision_only_sign_type_display4;
			u8 b7_vision_only_supplementary_sign_type_display4;

		};
		tsr_display* ptsr_display = (tsr_display*)pbuff;
	});
	g_msg_host.attach_monitor("details of lane", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF details_of_lane
		{
			u8 b0_lane_confidence_left : 2;
			u8 b0_ldw_available_left : 1;
			u8 b0_undocumented : 1;
			u8 b0_lane_type_left : 4;

			u8 b1_reserved : 4;
			u8 b1_distance_to_left_lane_lsb : 4;

			u8 b2_distance_to_left_lane_msb : 7;
			u8 b2_distance_to_left_lane_sg : 1;

			u8 b3_resreved;
			u8 b4_reserved;

			u8 b5_lane_confidence_right : 2;
			u8 b5_ldw_available_right : 1;
			u8 b5_undocumented : 1;
			u8 b5_lane_type_right : 4;

			u8 b6_reserved : 4;
			u8 b6_distance_to_right_lane_lsb : 4;

			u8 b7_distance_to_right_lane_msb : 7;
			u8 b7_distance_to_right_lane_sg : 1;

		};
		details_of_lane* pdata = (details_of_lane*)pbuff;
		left_lane_type = pdata->b0_lane_type_left;
		right_lane_type = pdata->b5_lane_type_right;
		s16 sleft_lane_dis = pdata->b2_distance_to_left_lane_msb * 16 + pdata->b1_distance_to_left_lane_lsb;
		if (pdata->b2_distance_to_left_lane_sg)
		{
			sleft_lane_dis = -sleft_lane_dis;
		}
		s16 sright_lane_dis = pdata->b7_distance_to_right_lane_msb * 16 + pdata->b6_distance_to_right_lane_lsb;
		if (pdata->b7_distance_to_right_lane_sg)
		{
			sright_lane_dis = -sright_lane_dis;
		}
		//printf("sleft_lane_dis=%d,sright_lane_dis=%d\n", sleft_lane_dis, sright_lane_dis);

	});
	g_msg_host.attach_monitor("lka left lane a", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF lka_left_lane_a
		{
			u8 b0_lane_type : 4;
			u8 b0_quality : 2;
			u8 b0_model_degree : 2;

			u8 b1_position_c0_byte0;

			u8 b2_position_c0_byte1;

			u8 b3_curvature_c2_byte0;

			u8 b4_curvature_c2_byte1;

			u8 b5_derivative_c3_byte0;

			u8 b6_derivative_c3_byte1;

			u8 b7_width_left_marking;
		};
		lka_left_lane_a* pdetails_lane = (lka_left_lane_a*)pbuff;
		//printf("left lane curve quality=%d lane_type=%d\n",pdetails_lane->b0_quality,pdetails_lane->b0_lane_type);
		if (pdetails_lane->b0_quality < 2 || pdetails_lane->b0_lane_type == en_lane_invalid)
		{
			return;
		}
		s16* plpos = (s16*)(pbuff + 1);
		left_position = *plpos / 256.f;
		//printf("lane leftpos=%f\n",left_position);
		left_curvature_stm = pdetails_lane->b3_curvature_c2_byte0 + pdetails_lane->b4_curvature_c2_byte1 * 0x100;
		calcu_left_right_lanes();
	});

	g_msg_host.attach_monitor("lka left lane b", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF lka_left_lane_b
		{
			u8 b0_heading_angle_byte0;

			u8 b1_heading_angle_byte1;

			u8 b2_view_range_lsb;

			u8 b3_view_range_msb : 7;
			u8 b3_view_range_availability : 1;

			u8 b4_reserve;
			u8 b5_reserve;
			u8 b6_reserve;
			u8 b7_reserve;
		};
		lka_left_lane_b* pdetails_lane = (lka_left_lane_b*)pbuff;
		u16* pheading = (u16*)pbuff;
		left_lane_heading_stm = *pheading;
		calcu_left_right_lanes();
	});
	g_msg_host.attach_monitor("lka right lane a", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF lka_right_lane_a
		{
			u8 b0_lane_type : 4;
			u8 b0_quality : 2;
			u8 b0_model_degree : 2;

			u8 b1_position_c0_byte0;

			u8 b2_position_c0_byte1;

			u8 b3_curvature_c2_byte0;

			u8 b4_curvature_c2_byte1;

			u8 b5_derivative_c3_byte0;

			u8 b6_derivative_c3_byte1;

			u8 b7_width_left_marking;
		};
		lka_right_lane_a* pdetails_lane = (lka_right_lane_a*)pbuff;
		if (pdetails_lane->b0_quality < 2 || pdetails_lane->b0_lane_type == en_lane_invalid)
		{
			return;
		}
		s16* plpos = (s16*)(pbuff + 1);
		right_position = *plpos / 256.f;
		//printf("lane rightpos=%f\n",right_position);
		right_curvature_stm = pdetails_lane->b3_curvature_c2_byte0 + pdetails_lane->b4_curvature_c2_byte1 * 0x100;
		calcu_left_right_lanes();
	});

	g_msg_host.attach_monitor("lka right lane b", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF lka_right_lane_b
		{
			u8 b0_heading_angle_byte0;

			u8 b1_heading_angle_byte1;

			u8 b2_view_range_lsb;

			u8 b3_view_range_msb : 7;
			u8 b3_view_range_availability : 1;

			u8 b4_reserve;
			u8 b5_reserve;
			u8 b6_reserve;
			u8 b7_reserve;
		};
		lka_right_lane_b* pdetails_lane = (lka_right_lane_b*)pbuff;
		u16* pheading = (u16*)pbuff;
		right_lane_heading_stm = *pheading;
		//printf("right_lane_heading_stm=%x\n", right_lane_heading_stm);
		calcu_left_right_lanes();
	});

	g_msg_host.attach_monitor("details - lane", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF details_lane
		{
			u8 b0_lane_cuevature_lsb;
			u8 b1_lane_cuevature_msb : 7;
			u8 b1_lane_cuevature_msb_sg : 1;

			u8 b2_lane_heading_lsb;

			u8 b3_lane_heading_msb : 4;
			u8 b3_ca : 1;
			u8 b3_right_ldw_availability : 1;
			u8 b3_left_ldw_availability : 1;
			u8 b3_na : 1;

			u8 b4_yaw_agle_lsb;
			u8 b5_yaw_agle_msb;

			u8 b6_pitch_angel_lsb;
			u8 b7_pitch_angel_msb;


		};
		details_lane* pdetails_lane = (details_lane*)pbuff;
	});
	g_msg_host.attach_monitor("details - obstacle status", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF details_obstacle_status
		{
			u8 b0_num_obstacles;

			u8 b1_timestamp;

			u8 b2_appliaction_vesion;

			u8 b3_avtive_version_number_section : 2;
			u8 b3_left_close_range_cut_in : 1;
			u8 b3_right_close_range_cut_in : 1;
			u8 b3_go : 4;

			u8 b4_protocol_version;

			u8 b5_reserved : 3;
			u8 b5_failsafe : 4;
			u8 b5_close_car : 1;
		};
		details_obstacle_status* pdata = (details_obstacle_status*)pbuff;
		obstacles_cnt = pdata->b0_num_obstacles;
		if (obstacles_cnt > 15)
		{
			obstacles_cnt = 15;
		}
		for (int ix = obstacles_cnt; ix < 15; ix++)
		{
			base_ui_component* pobj = obstacles_group->get_child(ix);
			pobj->set_visible(false);
		}
		if (obstacles_cnt == 0)
		{
			bool be_show = false;
			for (int ix = 0; ix < 6; ix++)
			{
				char seg_name[50] = "";
				sprintf(seg_name, "show_distance%d", ix);
				set_property_aliase_value(seg_name, &be_show);
			}
			for (int ix = 0; ix < 15; ix++)
			{
				base_ui_component* pobj = obstacles_group->get_child(ix);
				pobj->set_visible(false);
			}
		}
	});

	g_msg_host.attach_monitor("details - obstacle data a", [&](u8* pbuff, int len){
		u16* pmessg = (u16*)pbuff;
		int obstacle_id = (*pmessg - 0x739) / 3;
		pbuff += 2;
		struct GNU_DEF details_obstacle_data_a
		{
			u8 b0_obstacle_id;

			u16 b1_obstacle_pos_x : 12;

			u16 b2_null : 4;

			//u8 b3_obstacle_pos_y_lsb;

			//u8 b4_obstacle_pos_y_msb : 2;// 1;
			//u8 b4_obstacle_pos_y_sg : 1;
			s16 b34_obstacle_pos_y : 10;
			u8 b4_blinker_info : 3;
			u8 b4_cut_in_and_out : 3;

			u8 b5_obstacle_rel_vel_x_lsb;

			u8 b6_obstacle_rel_vel_x_msb;
			u8 b6_obstacle_type : 3;
			u8 b6_reserved : 1;

			u8 b7_obstacle_status : 3;
			u8 b7_obstacle_brake_lights : 1;
			u8 b7_obstacle_valid : 2;
			u8 b7_reserved : 2;

		};
		details_obstacle_data_a* pdata = (details_obstacle_data_a*)pbuff;
		/*pbuff += 3;
		printf("3rd byte=0x%x", *pbuff);
		pbuff++;
		printf(" 4th byte=0x%x\n", *pbuff);*/
		ft_image* pimg_obj = (ft_image*)obstacles_group->get_child(obstacle_id);
		if (obstacle_id < obstacles_cnt)
		{
			pimg_obj->set_visible(true);
		}
		else
		{
			pimg_obj->set_visible(false);
			return;
		}

		u16 posx = pdata->b1_obstacle_pos_x;
		//printf("obstacle_id=0x%x posx=0x%x\n", pdata->b0_obstacle_id, posx);
		if (posx == 0xfff)
		{
			printf("abandoned invalid xvalue\n");
			return;
		}
		if (posx > 0xfa0)
		{
			posx = 0xfa0;
		}
		if (posx > 0x200)
		{
			pimg_obj->set_texture_id(en_people_war0_png);
		}
		else
		if (posx > 0x100)
		{
			pimg_obj->set_texture_id(en_people_war1_png);
		}
		else
		{
			pimg_obj->set_texture_id(en_people_war2_png);
		}

		//const ImVec2 max_sz(69, 69),min_sz(9,9);
		//const float remotest_pos = 250, nearest_pos = 0.f;
		//float vscale=posx / 2500.0f;
		float vposx = -0.15*posx;
		float szu = 69 - posx * 0.05;
		//s16 uposy = pdata->b34_obstacle_pos_y; // (((u16)pdata->b4_obstacle_pos_y_msb )<<8 | pdata->b3_obstacle_pos_y_lsb)<<6;

		//printf("uposy=0x%x\n", uposy);

		s16 sposy = pdata->b34_obstacle_pos_y;// / 64;
		if (sposy >= 0x200)
		{
			printf("abandoned invalid yvalue\n");
			return;
		}
		float mz = posx*0.0625f;
		float mx = pdata->b34_obstacle_pos_y*0.0625;
		float my = 1.f;
#if 0
		g_from_xie.obj_input.x=mz;
		g_from_xie.obj_input.y=mx;
		g_from_xie.obj_input.z= -g_from_xie.ADAS_to_ground;
		if(Coordinate_Change(&g_from_xie) == OKAY)
		{
			pimg_obj->set_base_pos(g_from_xie.hud_obj_pixel_x,	g_from_xie.hud_obj_pixel_y);
			printf("output x=%f,y=%f\n",g_from_xie.hud_obj_pixel_x,g_from_xie.hud_obj_pixel_y);
			return;
		}
		return;
#endif

		//printf("sposy=%d\n", sposy);
		//sposy /= 64;

		float hpos = -sposy;
		//hpos -= 110;
		//printf("obstacle_id=%d,hscale=%f,hpos=%f,vposx=%f\n", obstacle_id, hscale, hpos, vposx);
		pimg_obj->set_base_pos(hpos, vposx);
		pimg_obj->set_size(ImVec2(szu, szu));
	});

	g_msg_host.attach_monitor("details - obstacle data b", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF details_obstacle_data_b
		{
			u8 b0_obstacle_length;

			u8 b1_obstacle_width;

			u8 b2_obstacle_age;

			u8 b3_obstacle_lane : 2;
			u8 b3_cipv_flag : 1;
			u8 b3_reserved : 1;
			u8 b3_rader_pos_x_lsb : 4;

			u8 b4_rader_pos_x_msb;

			u8 b5_radar_vel_x_lsb;

			u8 b6_radar_vel_x_msb : 4;
			u8 b6_radar_match_confidence : 3;
			u8 b6_reserved : 1;

			u8 b7_matched_radar_id;
			u8 b7_reserved : 1;

		};
		details_obstacle_data_b* pdetails_obstacle_data = (details_obstacle_data_b*)pbuff;
	});
	g_msg_host.attach_monitor("details - obstacle data c", [&](u8* pbuff, int len){
		pbuff += 2;
		struct GNU_DEF details_obstacle_data_c
		{
			u8 b0_obstacle_angle_rate_lsb;

			u8 b1_obstacle_angle_rate_msb;

			u8 b2_obstacle_scale_change_lsb;

			u8 b3_obstacle_scale_change_msb;

			u8 b4_object_accel_x;

			u8 b5_object_accel_x : 2;
			u8 b5_null : 2;
			u8 b5_obstacle_replaced : 1;
			u8 b5_reserved : 3;

			u8 b6_obstacle_angle_lsb;

			u8 b7_obstacle_angle_msb;

		};
		details_obstacle_data_c* pdetails_obstacle_data = (details_obstacle_data_c*)pbuff;
	});

}
	   

