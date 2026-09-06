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
    GLuint shadertemp = LoadShaders("mapshader.vert", "mapshader.frag");

    std::vector<float> tempvector;

    for (uint32_t i = 0; i < GlobalMapStuff->VisualPoints.size(); i++) {
      for (int j = 0; j < 3; j++)
        tempvector.push_back(GlobalMapStuff->VisualPoints[i].pos[j]);
      for (int j = 0; j < 3; j++)
        tempvector.push_back(GlobalMapStuff->VisualPoints[i].shade[j]);
    }

    std::vector<uint32_t> indices;

    for (uint32_t i = 0; i < GlobalMapStuff->Visualmapfaces.size(); i++) {
      for (int j = 0; j < 3; j++)
        indices.push_back(GlobalMapStuff->Visualmapfaces[i].points[j]);
    }

    RendererGlobal->GLstuff->shaders.push_back(shadertemp);

    glGenVertexArrays(1, &RendererGlobal->GLstuff->VAOthing);
    glGenBuffers(1, &RendererGlobal->GLstuff->VBOthing);
    glGenBuffers(1, &RendererGlobal->GLstuff->EBOthing);

    glBindVertexArray(RendererGlobal->GLstuff->VAOthing);

    glBindBuffer(GL_ARRAY_BUFFER, RendererGlobal->GLstuff->VBOthing);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * GlobalMapStuff->VisualPoints.size() * 6,
                 &tempvector[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererGlobal->GLstuff->EBOthing);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 GlobalMapStuff->Visualmapfaces.size() * sizeof(uint32_t) * 3,
                 &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
}