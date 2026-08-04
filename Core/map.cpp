#include <glad/glad.h>

#include <glm/glm.hpp>

#include "extern.h"
#include "files.h"

void LoadMapGL() {
  Global->GLstuff->MapGLlist = glGenLists(1);

  glNewList(Global->GLstuff->MapGLlist, GL_COMPILE);

  for (int i = 0; i < Global->mapfaces.size(); i++) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,
                  Global->GLstuff->textures[Global->mapfaces[i].texture]);
    glBegin(GL_TRIANGLES);
    for (int j = 2; j >= 0; j--) {
      glm::vec3 pos = Global->Points[Global->mapfaces[i].points[j]].pos;
      glm::vec2 uvw = Global->mapfaces[i].UVs[j];
      glColor3f(Global->Points[Global->mapfaces[i].points[j]].shade[0],
                Global->Points[Global->mapfaces[i].points[j]].shade[1],
                Global->Points[Global->mapfaces[i].points[j]].shade[2]);
      glTexCoord2f(uvw.x * Global->mapfaces[i].xloop,
                   uvw.y * Global->mapfaces[i].yloop);
      glVertex3f(pos.x, pos.y, pos.z);
    }
    glEnd();
  }

  glEnd();
  glEndList();
}