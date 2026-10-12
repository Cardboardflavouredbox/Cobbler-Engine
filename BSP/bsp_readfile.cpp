#include "bsp_readfile.h"

#include "map.h"
#include "mapdata.h"

MapBSP BSPResult;

// Source - https://stackoverflow.com/a/35396994
// Posted by Jomi
// Retrieved 2026-10-12, License - CC BY-SA 3.0
bool linePlaneIntersection(float& contact, glm::vec3 ray, glm::vec3 rayOrigin,
                           glm::vec3 normal, glm::vec3 coord) {
  // get d value
  float d = glm::dot(normal, coord);
  if (glm::dot(normal, ray) == 0) {
    return false;  // No intersection, the line is parallel to the plane
  }
  // Compute the X value for the directed line ray intersecting the plane
  float x = (d - glm::dot(normal, rayOrigin)) / glm::dot(normal, ray);
  // output contact point
  contact = x;  // Make sure your ray vector is normalized
  return true;
}

bool pointSideCheck(glm::vec3 point, glm::vec3 normal, glm::vec3 coord) {
  glm::vec3 toTarget = point - coord;

  float distance = glm::dot(toTarget, normal);

  return distance > 0.001f;
}

struct ProcessResult {
  std::vector<std::array<uint32_t, 3>> ResultFacesA, ResultFacesB;
  glm::vec3 normal, point;
  int cnt = 0;
};

ProcessResult ProcessFace(std::vector<std::array<uint32_t, 3>> Faces,
                          int index) {
  ProcessResult result;
  std::array<uint32_t, 3> currentface = Faces[index];

  glm::vec3 v1 = BSPResult.Points[currentface[1]] -
                 BSPResult.Points[currentface[0]],
            v2 = BSPResult.Points[currentface[2]] -
                 BSPResult.Points[currentface[0]];
  glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

  for (int a = 0; a < Faces.size(); a++) {
    if (index != a) {
      auto& face = Faces[a];
      float dummy;
      // if (!linePlaneIntersection(dummy, BSPResult.Points[face[1]],
      //                            BSPResult.Points[face[0]], normal,
      //                            BSPResult.Points[currentface[0]]) &&
      //     !linePlaneIntersection(dummy, BSPResult.Points[face[2]],
      //                            BSPResult.Points[face[0]], normal,
      //                            BSPResult.Points[currentface[0]]) &&
      //     !linePlaneIntersection(dummy, BSPResult.Points[face[2]],
      //                            BSPResult.Points[face[1]], normal,
      //                            BSPResult.Points[currentface[0]])) {
      //   // MapBSP::BSPnode node;
      //   // node.Face = BSPResult.Faces.size() - 1;
      //   // node.normal = result.normal;
      //   // node.point = result.point;
      //   // BSPResult.BSPtree.push_back(node);

      // }
      bool facecheck = false;
      for (int i = 0; i < 3; i++) {
        float temp, temp2;

        bool check = linePlaneIntersection(
                 temp, BSPResult.Points[face[(i + 1) % 3]],
                 BSPResult.Points[face[(i) % 3]], normal,
                 BSPResult.Points[currentface[0]]),
             check2 = linePlaneIntersection(
                 temp2, BSPResult.Points[face[(i + 2) % 3]],
                 BSPResult.Points[face[(i) % 3]], normal,
                 BSPResult.Points[currentface[0]]);
        if (check && check2 && temp >= 0 && temp <= 1 && temp2 >= 0 &&
            temp2 <= 1) {
          glm::vec3 A = BSPResult.Points[face[0]] +
                        glm::normalize(BSPResult.Points[face[(i + 1) % 3]]) *
                            temp,
                    B = BSPResult.Points[face[0]] +
                        glm::normalize(BSPResult.Points[face[(i + 2) % 3]]) *
                            temp2;

          if (glm::distance(A, B) > 0.001f &&
              glm::distance(A, BSPResult.Points[face[0]]) > 0.001f &&
              glm::distance(A, BSPResult.Points[face[1]]) > 0.001f &&
              glm::distance(A, BSPResult.Points[face[2]]) > 0.001f &&
              glm::distance(B, BSPResult.Points[face[0]]) > 0.001f &&
              glm::distance(B, BSPResult.Points[face[1]]) > 0.001f &&
              glm::distance(B, BSPResult.Points[face[2]]) > 0.001f) {
            BSPResult.Points.push_back(A);
            BSPResult.Points.push_back(B);

            std::array<std::array<uint32_t, 3>, 3> splitfaces;

            splitfaces[0][0] = face[i];
            splitfaces[0][1] = BSPResult.Points.size() - 1;
            splitfaces[0][2] = BSPResult.Points.size() - 2;

            splitfaces[1][0] = face[(i + 1) % 3];
            splitfaces[1][1] = face[(i + 2) % 3];
            splitfaces[1][2] = BSPResult.Points.size() - 1;

            splitfaces[2][0] = face[(i + 1) % 3];
            splitfaces[2][1] = BSPResult.Points.size() - 1;
            splitfaces[2][2] = BSPResult.Points.size() - 2;

            result.cnt += 1;

            for (int j = 0; j < 3; j++) {
              if (pointSideCheck(BSPResult.Points[splitfaces[j][0]], normal,
                                 BSPResult.Points[currentface[0]]))
                result.ResultFacesA.push_back(splitfaces[j]);
              else
                result.ResultFacesB.push_back(splitfaces[j]);
            }
            facecheck = true;
          }
          break;
        }
      }
      if (!facecheck) {
        if (pointSideCheck(BSPResult.Points[face[0]], normal,
                           BSPResult.Points[currentface[0]]))
          result.ResultFacesA.push_back(face);
        else
          result.ResultFacesB.push_back(face);
      }
    }
  }
  BSPResult.Faces.push_back(currentface);
  result.normal = normal;
  result.point = BSPResult.Points[currentface[0]];
  return result;
}

void BSPcreate(std::vector<std::array<uint32_t, 3>> Faces, bool isA,
               int parent) {
  if (Faces.size() > 1) {
    printf("%lu\n", Faces.size());
    ProcessResult result;
    for (int i = 0; i < Faces.size(); i++) {
      ProcessResult temp = ProcessFace(Faces, i);
      if (i == 0 ||
          (temp.ResultFacesA.size() > 0 && temp.ResultFacesB.size() > 0 &&
           (std::abs((int)result.ResultFacesA.size() -
                     (int)result.ResultFacesB.size()) +
                result.cnt * 4 >
            std::abs((int)temp.ResultFacesA.size() -
                     (int)temp.ResultFacesB.size()) +
                temp.cnt * 4))) {
        result = temp;
        // printf("%lu %lu\n", result.ResultFacesA.size(),
        //        result.ResultFacesB.size());
      }
    }
    MapBSP::BSPnode node;
    node.Face = BSPResult.Faces.size() - 1;
    node.normal = result.normal;
    node.point = result.point;
    BSPResult.BSPtree.push_back(node);
    int nodeindex = BSPResult.BSPtree.size() - 1;
    if (parent > -1) {
      if (isA)
        BSPResult.BSPtree[parent].A = nodeindex;
      else
        BSPResult.BSPtree[parent].B = nodeindex;
    }
    BSPcreate(result.ResultFacesA, true, nodeindex);
    BSPcreate(result.ResultFacesB, false, nodeindex);
  }
}

bool readfile(std::filesystem::path path) {
  Mapdata tempmapdata;
  FILE* file = NULL;

  // check if file is a .map file.
  if (path.extension() == ".map") {
    file = fopen(path.c_str(), "r");
  }

  if (file == NULL) {
    printf("Impossible to open the file!");
    return false;
  }
  // time to read the contents of the file.
  while (true) {
    char lineHeader;
    // read the first word of the line
    if (fscanf(file, "%c\n", &lineHeader) == EOF) break;

    if (lineHeader == 'V') {  // Visual thing
      VisualObject tempobject;
      while (true) {  // VisualBSPResult.Points.
        if (fscanf(file, "%c", &lineHeader) == EOF) break;
        if (lineHeader == 'E') break;
        glm::vec3 temppoint;
        fscanf(file, "%f,%f,%f\n", &temppoint.x, &temppoint.y, &temppoint.z);
        tempobject.VisualPoints.push_back(temppoint);
      }
      fscanf(file, "\n");
      while (true) {  // Visual Faces.
        if (fscanf(file, "%c", &lineHeader) == EOF) break;
        if (lineHeader == 'E') break;
        Mapface tempface;
        int doublesided;
        fscanf(file, "%d ", &doublesided);
        char subHeader;
        while (true) {
          if (fscanf(file, "%c", &subHeader) == EOF) break;
          if (subHeader == 'E') break;
          Mapface::TUVthing tuv;
          char texture[64];
          fscanf(file, "%s %f,%f %f,%f %f,%f ", texture, &tuv.UVs[0].x,
                 &tuv.UVs[0].y, &tuv.UVs[1].x, &tuv.UVs[1].y, &tuv.UVs[2].x,
                 &tuv.UVs[2].y);
          tuv.texture = texture;
          tempface.TUVvector.push_back(tuv);
        }

        fscanf(file, "%u,%u,%u\n", &tempface.points[0], &tempface.points[1],
               &tempface.points[2]);

        tempface.doublesided = doublesided;
        for (int i = 0; i < 3; i++) tempface.points[i];
        tempobject.Visualmapfaces.push_back(tempface);
      }
      fscanf(file, "\n");
      tempmapdata.VisualObjectsVector.push_back(tempobject);
    } else if (lineHeader == 'H') {  // Hitbox thing
      uint32_t cnt = tempmapdata.HitboxPoints.size();
      while (true) {  // HitboxBSPResult.Points.
        if (fscanf(file, "%c", &lineHeader) == EOF) break;
        if (lineHeader == 'E') break;
        glm::vec3 temppoint;
        fscanf(file, "%f,%f,%f\n", &temppoint.x, &temppoint.y, &temppoint.z);
        tempmapdata.HitboxPoints.push_back(temppoint);
      }
      fscanf(file, "\n");
      while (true) {  // Hitbox Faces.
        if (fscanf(file, "%c", &lineHeader) == EOF) break;
        if (lineHeader == 'E') break;
        std::array<uint32_t, 3> tempface;
        fscanf(file, "%u,%u,%u\n", &tempface[0], &tempface[1], &tempface[2]);

        for (int i = 0; i < 3; i++) tempface[i] += cnt;
        tempmapdata.Hitboxmapfaces.push_back(tempface);
      }
      fscanf(file, "\n");
    } else if (lineHeader == 'K') {  // Killbox thing
      uint32_t cnt = tempmapdata.KillboxPoints.size();
      while (true) {  // KillboxBSPResult.Points.
        if (fscanf(file, "%c", &lineHeader) == EOF) break;
        if (lineHeader == 'E') break;
        glm::vec3 temppoint;
        fscanf(file, "%f,%f,%f\n", &temppoint.x, &temppoint.y, &temppoint.z);
        tempmapdata.KillboxPoints.push_back(temppoint);
      }
      fscanf(file, "\n");
      while (true) {  // Killbox Faces.
        if (fscanf(file, "%c", &lineHeader) == EOF) break;
        if (lineHeader == 'E') break;
        std::array<uint32_t, 3> tempface;
        fscanf(file, "%u,%u,%u\n", &tempface[0], &tempface[1], &tempface[2]);

        for (int i = 0; i < 3; i++) tempface[i] += cnt;
        tempmapdata.KillboxFaces.push_back(tempface);
      }
      fscanf(file, "\n");
    } else if (lineHeader == 'C') {  // Skybox.
      char name[64];
      fscanf(file, "%s", name);
      tempmapdata.skybox = name;
      break;
    } else if (lineHeader == 'S') {  // SpawnPoints.
      glm::vec3 pos;
      fscanf(file, "%f,%f,%f\n", &pos.x, &pos.y, &pos.z);
      tempmapdata.SpawnPoints.push_back(pos);
    }
  }
  fclose(file);

  BSPResult.Points = tempmapdata.HitboxPoints;
  std::vector<std::array<uint32_t, 3>> Faces = tempmapdata.Hitboxmapfaces;
  BSPcreate(Faces, false, -1);

  std::string paththing = path.string();
  for (int i = 0; i < 3; i++) paththing.pop_back();
  paththing += "bsp";

  file = fopen(paththing.c_str(), "w");

  if (file == NULL) {
    printf("Impossible to write the file!");
    return false;
  }

  printf("writing to %s", paththing.c_str());

  fprintf(file, "V\n");
  for (int i = 0; i < BSPResult.Points.size(); i++) {
    fprintf(file, "%f %f %f\n", BSPResult.Points[i][0], BSPResult.Points[i][1],
            BSPResult.Points[i][2]);
  }
  fprintf(file, "E\n");

  fprintf(file, "F\n");
  for (int i = 0; i < BSPResult.Faces.size(); i++) {
    fprintf(file, "%u %u %u\n", BSPResult.Faces[i][0], BSPResult.Faces[i][1],
            BSPResult.Faces[i][2]);
  }
  fprintf(file, "E\n");

  fprintf(file, "B\n");
  for (int i = 0; i < BSPResult.BSPtree.size(); i++) {
    MapBSP::BSPnode& bspthing = BSPResult.BSPtree[i];
    fprintf(file, "%d %d %u %f %f %f %f %f %f\n", bspthing.A, bspthing.B,
            bspthing.Face, bspthing.normal[0], bspthing.normal[1],
            bspthing.normal[2], bspthing.point[0], bspthing.point[1],
            bspthing.point[2]);
  }
  fprintf(file, "E");

  fclose(file);

  return true;
}