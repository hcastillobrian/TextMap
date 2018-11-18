/////////////////////////////////////////////
// TextMap examples 
// developed by Hernan Castillo Brian
//

#include <stdio.h>

#include "../include/definitions.h"
#include "../include/TextMap.h"

using namespace FFe;

void read_file_to_string(const String& file, String& string_out)
{
	auto fr = fopen(file.c_str(), "rb");
	if (fr)
	{
		fseek(fr, 0, SEEK_END);
		string_out.resize(ftell(fr));
		fseek(fr, 0, SEEK_SET);
		fread(&string_out[0], string_out.size(), sizeof(char), fr);
		fclose(fr);
	}
}
void write_string_to_file(const String& string_in, const String& file)
{
	auto fw = fopen(file.c_str(), "wb");
	if (fw)
	{
		fwrite(&string_in[0], string_in.size(), sizeof(char), fw);
		fclose(fw);
	}
}

void MeshRead()
{
	String data;
	read_file_to_string("examples_data/FilmMesh.txt", data);

	struct Vertex
	{
		float pos[3];
		float tex[2];
	};
	Vector<Vertex> vertices;
	Vector<int> indices;

	TextMap tmMesh(data);
	{
		auto tm = tmMesh / "Vertex";
		vertices.resize(tm.Count());
		for (unsigned i = 0; i < vertices.size(); i++)
		{
			auto& tmv = tm / i / "v";
			for (int k = 0; k < 3; k++) vertices[i].pos[k] = tmv / k;
			auto& tmt = tm / i / "t";
			for (int k = 0; k < 2; k++) vertices[i].pos[k] = tmt / k;
		}
	}

	{
		auto tm = tmMesh / "Face";
		indices.resize(tm.Count() * 3);
		for (int i = 0; i < tm.Count(); i++)
		{
			auto& tmi = tm / i / "i";
			indices[i * 3 + 0] = tmi / 0;
			indices[i * 3 + 1] = tmi / 1;
			indices[i * 3 + 2] = tmi / 2;
		}
	}
}


class Settings
{
public:
	String language;
	float volume;
	bool is_enhanced;
	bool is_subtitles;
	String adapter;
	String output;
	bool is_fullscreen;
	bool is_vsync;
	bool is_allvideomodes;
	int resolution_width;
	int resolution_height;

public:
	void read()
	{
		String data;
		read_file_to_string("examples_data/Config.txt", data);

		TextMap tm(data);
		language = (tm / "Language" / 0).As("English");
		volume = (tm / "Volume" / 0).As(0.8f);
		is_enhanced = (tm / "Enhanced" / 0).As(true);
		is_subtitles = (tm / "Subtitles" / 0).As(true);

		auto tmVideo = tm / "VideoSettings";
		adapter = tmVideo / "Device" / 0;
		output = tmVideo / "Display" / 0;
		resolution_width = tmVideo / "Resolution" / 0;
		resolution_height = tmVideo / "Resolution" / 0;
		is_fullscreen = (tmVideo / "Fullscreen" / 0).As(true);
		is_vsync = (tmVideo / "VSync" / 0).As(true);
		is_allvideomodes = (tmVideo / "AllModes" / 0).As(false);
	}
	void write()
	{
		TextMap tm;
		tm + "Language" < language;
		tm + "Volume" < volume;
		tm + "Enhanced" < is_enhanced;
		tm + "Subtitles" < is_subtitles;

		auto tmVideo = tm + "VideoSettings";
		tmVideo + "Device" < adapter;
		tmVideo + "Display" < output;
		tmVideo + "Resolution" < resolution_width < resolution_height;
		tmVideo + "Fullscreen" < is_fullscreen;
		tmVideo + "VSync" < is_vsync;
		tmVideo + "AllModes" < is_allvideomodes;

		write_string_to_file(tm.ToString(), "examples_data/Config.txt");
	}

};

int main()
{
	MeshRead();


	{
		Settings my_settings;
		my_settings.read();

		my_settings.is_fullscreen = false;
		my_settings.volume = (float)(rand() % 100) / 100.0f;
		my_settings.resolution_width = rand() % 1920;
		my_settings.resolution_height = rand() % 1080;
		my_settings.language = "English";

		my_settings.write();
	}

    return 0;
}

