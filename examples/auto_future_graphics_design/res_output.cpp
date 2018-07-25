#pragma once
#include "res_output.h"

vres_txt_list  g_vres_texture_list;
int g_cur_texture_id_index=0;
mtexture_list g_mtexture_list;

af_file::af_file(GLuint fsize)
	:_fsize(fsize)
{
	_pbin = malloc(fsize);
}
af_file::~af_file()
{
	free(_pbin);
}
mfile_list g_mfiles_list;
#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)
#include "SOIL.h"
#include <fstream>
void add_image_to_mtexure_list(string& imgPath)
{
	string img_file_name = imgPath.substr(imgPath.find_last_of('\\') + 1);
	string img_file_path = imgPath.substr(0, imgPath.find_last_of('\\') + 1);

	GLubyte* imgdata = NULL;
	int width, height, channels;

	imgdata = SOIL_load_image(imgPath.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
	if (imgdata == NULL)
	{
		printf("Fail to load texture file:%s\n", imgPath.c_str());
		return;
	}
	extern string g_cureent_project_file_path;
	string str_img_path = g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
	str_img_path += "images\\";
	if (img_file_path != str_img_path)
	{
		string str_cmd = "copy ";
		str_cmd += imgPath;
		str_cmd += " ";
		str_cmd += str_img_path;
		system(str_cmd.c_str());

	}
	GLuint textureId = 0;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	if (channels == SOIL_LOAD_RGBA)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	// Step2 �趨wrap����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Step3 �趨filter����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR); // ΪMipMap�趨filter����
	// Step4 ��������

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, imgdata);
	glGenerateMipmap(GL_TEXTURE_2D);
	// Step5 �ͷ�����ͼƬ��Դ
	SOIL_free_image_data(imgdata);
	auto pimge = make_shared<af_texture>();
	pimge->_txt_id = textureId;
	pimge->_width = width;
	pimge->_height = height;
	g_mtexture_list[img_file_name] = pimge;
}
void add_file_to_mfiles_list(string& file_path)
{
	string str_file = file_path;
	string strfile_name = str_file.substr(str_file.find_last_of('\\') + 1);
	string str_file_path = str_file.substr(0, str_file.find_last_of('\\') + 1);
	extern string g_cureent_project_file_path;
	string af_file_path = g_cureent_project_file_path.substr(0, g_cureent_project_file_path.find_last_of('\\') + 1);
	af_file_path += "files\\";
	if (str_file_path != af_file_path)
	{
		string str_cmd = "copy ";
		str_cmd += str_file;
		str_cmd += " ";
		str_cmd += af_file_path;
		system(str_cmd.c_str());

	}
	ifstream ifs;
	ifs.open(file_path);
	if (!ifs.is_open())
	{
		MessageBox(GetForegroundWindow(), "fail to load file", "Error info", MB_OK);
		return;
	}
	filebuf* pbuf = ifs.rdbuf();
	size_t sz_file = pbuf->pubseekoff(0, ifs.end, ifs.in);
	pbuf->pubseekpos(0, ifs.in);
	g_mfiles_list[strfile_name] = make_shared<af_file>(sz_file);
	pbuf->sgetn((char*)g_mfiles_list[strfile_name]->_pbin, sz_file);
	ifs.close();
}
#endif