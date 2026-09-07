#include "map.h"

#include <SDL3/SDL_log.h>
#include <glad/gl.h>

#include <glm/glm.hpp>

#include "files.h"
#include "render.h"

void LoadMapGL(bool IsOldGL) {
  if (IsOldGL) {
    RendererGlobal->GLstuff->MapGLlist = glGenLists(1);
    glNewList(RendererGlobal->GLstuff->MapGLlist, GL_COMPILE);

    for (int i = 0; i < GlobalMapStuff->Visualmapfaces.size(); i++) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D,
                    RendererGlobal->GLstuff
                        ->textures[GlobalMapStuff->Visualmapfaces[i].texture]);
      glBegin(GL_TRIANGLES);
      for (int j = 2; j >= 0; j--) {
        glm::vec3 pos =
            GlobalMapStuff
                ->VisualPoints[GlobalMapStuff->Visualmapfaces[i].points[j]]
                .pos;
        glm::vec2 uvw = GlobalMapStuff->Visualmapfaces[i].UVs[j];
        glColor3f(
            GlobalMapStuff
                ->VisualPoints[GlobalMapStuff->Visualmapfaces[i].points[j]]
                .shade[0],
            GlobalMapStuff
                ->VisualPoints[GlobalMapStuff->Visualmapfaces[i].points[j]]
                .shade[1],
            GlobalMapStuff
                ->VisualPoints[GlobalMapStuff->Visualmapfaces[i].points[j]]
                .shade[2]);
        glTexCoord2f(uvw.x, 1 - uvw.y);
        glVertex3f(pos.x, pos.y, pos.z);
      }
      glEnd();
    }

    glEnd();
    glEndList();
  } else {
    RendererStuff::OpenGLRenderer::GLObject globjectthing;

    globjectthing.texture = RendererGlobal->GLstuff->textures["texture"];

    GLuint shadertemp = LoadShaders("mapshader.vert", "mapshader.frag");

    RendererGlobal->GLstuff->shaders.push_back(shadertemp);

    std::vector<float> vertices;

    for (auto& i : GlobalMapStuff->Visualmapfaces) {
      for (int a = 2; a >= 0; a--) {
        for (int j = 0; j < 3; j++) {
          vertices.push_back(GlobalMapStuff->VisualPoints[i.points[a]].pos[j]);
        }
        for (int j = 0; j < 3; j++) {
          vertices.push_back(
              GlobalMapStuff->VisualPoints[i.points[a]].shade[j]);
        }
        for (int j = 0; j < 2; j++) {
          vertices.push_back(i.UVs[a][j]);
        }
        globjectthing.size++;
      }
    }
    glGenVertexArrays(1, &globjectthing.VAOthing);
    glGenBuffers(1, &globjectthing.VBOthing);

    glBindVertexArray(globjectthing.VAOthing);

    glBindBuffer(GL_ARRAY_BUFFER, globjectthing.VBOthing);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * globjectthing.size * 8,
                 &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    RendererGlobal->GLstuff->GlObjects.push_back(globjectthing);
  }
}