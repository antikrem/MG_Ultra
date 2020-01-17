/*vao for point lights*/
#ifndef __VAO_POINT_LIGHT__
#define __VAO_POINT_LIGHT__

#include "screen_space_consts.h"

#include "vao_buffer.h"

#include "point_light.h"

#include "geometry_kit.h"

#include "ini_parser.h"

class VAOPointLight : public VAOBuffer<VAOPointLight, PointLightData> {
	//sphere vbo
	GLuint sphereVBO;

	//sphere ibo
	GLuint sphereIBO;

	//number of indexes
	int indexCount;

public:
	void setUpAttribPointers() {

		//create sphere
		vector<float> vertices;
		vector<int> indices;

		{
			INIParser gfx("settings/gfx_settings.ini");

			geom_kit::populate_sphere(
				vertices,
				indices,
				gfx.get("lighting", "pointlight_stacks", 8),
				gfx.get("lighting", "pointlight_sectors", 14),
				1.0f
			);
		}

		indexCount = (int)indices.size();

		//Generate and bind base spehere
		glGenBuffers(1, &sphereVBO);
		glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

		//Generate and bind base quad buffer
		glGenBuffers(1, &sphereIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * indices.size(), indices.data(), GL_STATIC_DRAW);

		//Generate and bind instance buffer
		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, maxSize * sizeof(PointLightData), NULL, GL_DYNAMIC_DRAW);

		//set up base vertex pointer
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
		glVertexAttribPointer(
			0, //layout = 0
			3, //vec3
			GL_FLOAT,
			GL_FALSE,
			0, //no need to stride
			(void*)0 //vertexes starting at 0
		);

		//set up position pointer
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glVertexAttribPointer(
			1, //layout = 1
			3, //vec3
			GL_FLOAT,
			GL_FALSE,
			sizeof(PointLightData), //stride the length of boxdata
			(void*)(offsetof(PointLightData, PointLightData::position)) //position part of point lighting
		);

		//set up colour pointer
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glVertexAttribPointer(
			2, //layout = 2
			3, //vec3
			GL_FLOAT,
			GL_FALSE,
			sizeof(PointLightData), //stride the length of boxdata
			(void*)(offsetof(PointLightData, PointLightData::colour)) //colour part of point lighting
		);

		//a const
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glVertexAttribPointer(
			3, //layout = 3
			1, //float
			GL_FLOAT,
			GL_FALSE,
			sizeof(PointLightData), //stride the length of boxdata
			(void*)(offsetof(PointLightData, PointLightData::a)) //a const
		);

		//b const
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glVertexAttribPointer(
			4, //layout = 4
			1, //float
			GL_FLOAT,
			GL_FALSE,
			sizeof(PointLightData), //stride the length of boxdata
			(void*)(offsetof(PointLightData, PointLightData::b)) //b const
		);

		//c const
		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glVertexAttribPointer(
			5, //layout = 5
			1, //float
			GL_FLOAT,
			GL_FALSE,
			sizeof(PointLightData), //stride the length of boxdata
			(void*)(offsetof(PointLightData, PointLightData::c)) //c const
		);

		//range
		glEnableVertexAttribArray(6);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glVertexAttribPointer(
			6, //layout = 6
			1, //float
			GL_FLOAT,
			GL_FALSE,
			sizeof(PointLightData), //stride the length of boxdata
			(void*)(offsetof(PointLightData, PointLightData::extinctionRange)) //range
		);

		//set up striding
		glVertexAttribDivisor(0, 0); //base sphere vertexes
		glVertexAttribDivisor(1, 1); //position
		glVertexAttribDivisor(2, 1); //colour
		glVertexAttribDivisor(3, 1); //a
		glVertexAttribDivisor(4, 1); //b
		glVertexAttribDivisor(5, 1); //c
		glVertexAttribDivisor(6, 1); //range
	}

	void render(int count) {
		//draw point lights, count is the number of lights
		glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, count);
	}


	VAOPointLight(int maxSize) 
	: VAOBuffer<VAOPointLight, PointLightData>(maxSize) {
	}
};

#endif