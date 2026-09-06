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

    RendererGlobal->GLstuff->shaders.push_back(shadertemp);

    glGenBuffers(1, &RendererGlobal->GLstuff->VBOthing);
    glBindBuffer(GL_ARRAY_BUFFER, RendererGlobal->GLstuff->VBOthing);

    glGenVertexArrays(1, &RendererGlobal->GLstuff->VAOthing);
    glBindVertexArray(RendererGlobal->GLstuff->VAOthing);

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(MapPoint) * GlobalMapStuff->VisualPoints.size(),
                 GlobalMapStuff->VisualPoints.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)offsetof(MapPoint, pos));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)offsetof(MapPoint, shade));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
}