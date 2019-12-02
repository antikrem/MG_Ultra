/*Screenspace constants for screen space rendering*/
#ifndef __SCRREN_SPACE_CONSTS__
#define __SCRREN_SPACE_CONSTS__

//screen space quad
const float SCREEN_SPACE_QUAD[] = {
	-1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f
};

//uvs for screen space
const float SCREEN_SPACE_UV[] = {
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f
};

#endif