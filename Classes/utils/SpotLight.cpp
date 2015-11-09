#include "SpotLight.h"

SpotLight * SpotLight::create(RenderTexture * texture, float radius, Color4F color){
	SpotLight * spotLight=new SpotLight();
	spotLight->init(texture, radius, color);
	spotLight->autorelease();
	return spotLight;
}

bool SpotLight::init(RenderTexture * texture, float radius, Color4F color){
	if (Sprite::init()) {
		this->renderTexture = texture;
		this->spotLightRadius = radius;
		this->renderColor = color;
		this->originRadius=radius;
		symbolFlag=1;
		return true;
	}
	return false;
}

void SpotLight::onEnterTransitionDidFinish(){
	Sprite::onEnterTransitionDidFinish();
	this->scheduleUpdate();
}

void SpotLight::update(float delta)
{
	//¶¶¶¯ spotLightRadius
	if ( spotLightRadius> originRadius*0.05+originRadius) {
		symbolFlag=-1;
	}else if (spotLightRadius < originRadius*0.95){
		symbolFlag=1;
	}

	spotLightRadius = spotLightRadius + symbolFlag*1;
	const int segs = 45;
	/*GLfloat *vertices = (GLfloat *)malloc(sizeof(GLfloat)*2*(segs));
	GLfloat *coordinates = (GLfloat *)malloc(sizeof(GLfloat)*2*(segs));
	ccColor4B *colors = (ccColor4B *)malloc(sizeof(ccColor4B)*(segs));*/

	GLfloat vertices[2 * segs + 2];//(GLfloat *)malloc(sizeof(GLfloat)*2*(segs));
	GLfloat coordinates[2 * segs + 2];//(GLfloat *)malloc(sizeof(GLfloat)*2*(segs));
	ccColor4B colors[segs];//(ccColor4B *)malloc(sizeof(ccColor4B)*(segs));

	//memset(vertices,0, sizeof(GLfloat)*2*(segs));
	//memset(coordinates,0, sizeof(GLfloat)*2*(segs));
	for(auto i : vertices)
		i = 0;
	for(auto i : coordinates)
		i = 0;

	renderTexture->clear(renderColor.r, renderColor.g, renderColor.b, renderColor.a);

	colors[0] = ccc4(0, 0, 0, 255);
	for (int i = 1; i < segs; i++)
	{
		colors[i] = ccc4(0, 0, 0, 0);
	}

	const float coef = 2.0f * (float)M_PI/(segs-2) ;

	vertices[0] = this->getPosition().x;
	vertices[1] = this->getPosition().y;
	coordinates[0] = this->getPosition().x;
	coordinates[1] = (this->getContentSize().height-this->getPosition().y);
	for(int i=1;i<=segs;i++)
	{
		float rads = i*coef;
		float j =this->spotLightRadius * cosf(rads) + this->getPosition().x;
		float k =this->spotLightRadius * sinf(rads) + this->getPosition().y;
		vertices[i*2] = j;
		vertices[i*2+1] = k;
		coordinates[i*2] = (j);
		coordinates[i*2+1] = (this->getContentSize().height-k);
	}

	// Update the render texture
	renderTexture->begin();

	glBindTexture(GL_TEXTURE_2D, (GLuint)this->renderTexture);
	glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
	glColorMask(0.0f, 0.0f, 0.0f, 1.0f);

	// Pass the verticies to draw to OpenGL
	glEnableVertexAttribArray(kCCVertexAttribFlag_Position);
	glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_TRUE, 0, vertices);

	// Pass the verticies to draw texture
	glEnableVertexAttribArray(kCCVertexAttribFlag_TexCoords);
	glVertexAttribPointer(kCCVertexAttribFlag_TexCoords, 2, GL_FLOAT, GL_TRUE, 0, coordinates);

	// Pass the colors of the vertices to draw to OpenGL
	glEnableVertexAttribArray(kCCVertexAttribFlag_Color);
	glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, colors);

	glDrawArrays(GL_TRIANGLE_FAN, 0, segs);

	glColorMask(1.0f, 1.0f, 1.0f, 1.0f);
	glBlendFunc(CC_BLEND_SRC, CC_BLEND_DST);

	renderTexture->end();

	//free(vertices);
	//free(coordinates);
	//free(colors);
}