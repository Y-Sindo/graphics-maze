#include <GL/glew.h>
#include "GL/glut.h"
#include "stdlib.h"
#include "glm.h"
#include "obj.h"

extern GLuint tv, lamp, coin;

void loadOBJ(){
	tv = drawOBJ("OBJ/Tv/tv.obj");
	coin = drawOBJ("OBJ/Coin/TyveKrone.obj");
}

GLuint drawOBJ(char * filename){
	GLMmodel *glm_model;
	GLuint list;
	glm_model = glmReadOBJ(filename);
	glmUnitize(glm_model);
	glmScale(glm_model, 5);
	glmFacetNormals(glm_model);
	glmVertexNormals(glm_model, 90);
	list = glmList(glm_model, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL );

	glmDelete(glm_model);
	return list;
}
