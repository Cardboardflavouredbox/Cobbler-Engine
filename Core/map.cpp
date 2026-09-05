#include "map.h"

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "files.h"
#include "render.h"

void LoadMapGL() {
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
      glColor3f(GlobalMapStuff
                    ->VisualPoints[GlobalMapStuff->Visualmapfaces[i].points[j]]
                    .shade[0],
                GlobalMapStuff
                    ->VisualPoints[GlobalMapStuff->Visualmapfaces[i].points[j]]
                    .shade[1],
                GlobalMapStuff
                    ->VisualPoints[GlobalMapStuff->Visualmapfaces[i].points[j]]
                    .shade[2]);
      glTexCoord2f(uvw.x, uvw.y);
      glVertex3f(pos.x, pos.y, pos.z);
    }
    glEnd();
  }

  glEnd();
  glEndList();
}