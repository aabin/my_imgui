#pragma once
#include <string>
#include <vector>
#include <memory>
#include <stddef.h>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

struct field_ele
{
	std::string _type;
	std::string _name;
	int _tpsz;
	int _offset;
	field_ele(std::string tp, std::string nm, int tpsz, int offset)
		:_type(tp), _name(nm),_tpsz(tpsz),_offset(offset){}
};
typedef std::shared_ptr<field_ele> sp_field_ele;
//
typedef std::vector<sp_field_ele> vt_field_ele;
struct  prop_ele
{
	void* _pro_address;
	vt_field_ele _pro_page;
	prop_ele(void* paddress) :_pro_address(paddress){}
};
#ifdef _MSC_VER 
#define MSC_PACK_HEAD __pragma(pack(push, 1))
#define MSC_PACK_END __pragma(pack(pop))
#define GNU_DEF
#elif defined(__GNUC__)
#define MSC_PACK_HEAD 
#define MSC_PACK_END 
 #define GNU_DEF __attribute__((packed)) 
#endif
typedef std::shared_ptr<prop_ele> sp_prop_ele;
typedef std::vector<sp_prop_ele> vp_prop_ele;
#define GET_OFFSET(st,mb) offsetof(st,mb)
#define _PIKSTR(x) #x
#define PIKSTR(x) _PIKSTR(x)
#define EMP_IN_PPT(x)	ppt->_pro_page.emplace_back(make_shared<field_ele>(x));
#define SPLIT_ELE(ele) PIKSTR(BOOST_PP_TUPLE_ELEM(2,0,ele)),PIKSTR(BOOST_PP_TUPLE_ELEM(2,1,ele))
#define GET_CT_PM(ele,st) SPLIT_ELE(ele),sizeof(BOOST_PP_TUPLE_ELEM(2,0,ele)),GET_OFFSET(st,BOOST_PP_TUPLE_ELEM(2,1,ele))
#define GET_ELE(r,data,ele) EMP_IN_PPT(GET_CT_PM(ele,data))
#define GEN_V(r,data,elem) BOOST_PP_TUPLE_ELEM(2,0,elem) BOOST_PP_TUPLE_ELEM(2,1,elem);
#define DEF_STRUCT(stname,vname,...) MSC_PACK_HEAD \
 struct GNU_DEF stname {\
	BOOST_PP_SEQ_FOR_EACH(GEN_V,_,BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\
	stname(){}\
	stname(vp_prop_ele& vprop_ele){sp_prop_ele ppt=make_shared<prop_ele>(this);\
	BOOST_PP_SEQ_FOR_EACH(GET_ELE,stname,BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))\
	vprop_ele.emplace_back(ppt);\
    }\
};\
stname vname{_vprop_eles}; MSC_PACK_END
/************************************************************************/
/* 
ʹ��DEF_STRUCT������ṹ��ע�����
1�������Ա����ֱ�ӳ�ʼ�������磺char a[20]{1,2,3}; //error C2536��
2�������������͵ĳ�Ա����Ĭ�ϵ����Բ������ͻ����͵ĳ�Ա���Բ���,������ע��ص����������ṩ�������ơ�
3���û���������ض��Ľṹ������ṩ���Բ�����������ע��ص�������ͬʱ�ṩ�ṹ������ĵ�ַ��
4���û���������ض��Ľṹ���Ա�����ṩ���Բ�����������ע��ص�������ͬʱ�ṩ�ṹ������ĵ�ַ�ͽṹ���Ա�����ڸýṹ�е�λ��������
*/
/************************************************************************/