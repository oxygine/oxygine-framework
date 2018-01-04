#pragma once
#include "test.h"


struct myVertex
{
    Vector3 pos;
    Vector2 uv;
};

DECLARE_SMART(OpenGLSprite, spOpenGLSprite);
class OpenGLSprite : public Sprite
{
public:

    int _program;

    OpenGLSprite()
    {
        const char* vertexShaderData = R"(
            uniform mediump mat4 projection;
            attribute vec3 a_position;
            attribute vec2 a_uv;
            varying mediump vec2 v_uv;
            void main() 
            {
                vec4 position = vec4(a_position, 1.0);
                gl_Position = projection * position;
                v_uv = a_uv;
            }
		)";

        const char* fragmentShaderData = R"(
            varying mediump vec2 v_uv;
            uniform lowp sampler2D base_texture;
                                      
			void main() 
            {
			    gl_FragColor = texture2D(base_texture, v_uv);
			}
		)";


        int vs = ShaderProgramGL::createShader(GL_VERTEX_SHADER,   vertexShaderData);
        int fs = ShaderProgramGL::createShader(GL_FRAGMENT_SHADER, fragmentShaderData);

        _program = oxglCreateProgram();
        oxglAttachShader(_program, vs);
        oxglAttachShader(_program, fs);


        CHECKGL();

        oxglBindAttribLocation(_program, 0, "a_position");
        oxglBindAttribLocation(_program, 1, "a_uv");

        CHECKGL();

        oxglLinkProgram(_program);

        oxglDetachShader(_program, vs);
        oxglDetachShader(_program, fs);

        oxglDeleteShader(vs);
        oxglDeleteShader(fs);


        CHECKGL();
    }

    ~OpenGLSprite()
    {
        oxglDeleteProgram(_program);
    }

    void doRender(const RenderState& rs)
    {
        Material::null->apply();


        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);



        IVideoDriver* driver = IVideoDriver::instance;
        oxglUseProgram(_program);


        oxglActiveTexture(GL_TEXTURE0);

        AnimationFrame frame = resources.getResAnim("bg")->getFrame(0);
        const RectF& srcRect = frame.getSrcRect();

        int texture = (int)(size_t)frame.getDiffuse().base->getHandle();

        glBindTexture(GL_TEXTURE_2D, texture);


        CHECKGL();

        Matrix m = Matrix(rs.transform) * STDRenderer::instance->getViewProjection();

        int projLocation = oxglGetUniformLocation(_program, "projection");
        oxglUniformMatrix4fv(projLocation, 1, GL_FALSE, m.ml);

        CHECKGL();

        myVertex vertices[4];
        myVertex* v = vertices;

        v->pos = Vector3(-100, -100, 0);
        v->uv = srcRect.getLeftTop();
        ++v;

        v->pos = Vector3(100, -100, 0);
        v->uv = srcRect.getRightTop();
        ++v;

        v->pos = Vector3(100, 100, 0);
        v->uv = srcRect.getRightBottom();
        ++v;

        v->pos = Vector3(-100, 100, 0);
        v->uv = srcRect.getLeftBottom();
        ++v;


        oxglEnableVertexAttribArray(0);
        oxglVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(myVertex), vertices);

        oxglEnableVertexAttribArray(1);
        oxglVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(myVertex), (float*)vertices + 3);



        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


        oxglDisableVertexAttribArray(0);
        oxglDisableVertexAttribArray(1);

        CHECKGL();

        //reset states to defaults after direct opengl usage
        rsCache().reset();
    }
};


class TestOpenGL : public Test
{
public:
    TestOpenGL()
    {
        spOpenGLSprite sprite = new OpenGLSprite;
        sprite->setPosition(getStage()->getSize() / 2);
        sprite->setScale(3);
        content->addChild(sprite);
    }
};
