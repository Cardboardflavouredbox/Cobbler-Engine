#include "commandline.h"

#include <SDL3/SDL_log.h>

#include <unordered_map>

#include "extern.h"
#include "netconnect.h"
#include "network.h"
#include "networkextern.h"
#include "settings.h"

// enums for arguements.
enum cmdenums {
  SetRendererAsVulkan,    // Sets Renderer to Vulkan.
  SetRendererAsOpenGL,    // Sets Renderer to OpenGL.
  SetRendererAsSoftware,  // THIS WILL BE REMOVED EVENTUALLY.
  SetFPS,                 // Sets Frame rate.
  SetFOV,                 // Sets field of view.
  SetVsync,               // Sets Vsync
  SetServerIP,            // Set the IP of the server as a client.
  SetIsServer,            // Set if you are the server or not.
  SetIsDebug              // Sets Debug.
};

bool is_number(const std::string& s) {
  for (int i = 0; i < s.size(); i++) {
    if (!std::isdigit(s[i])) return false;
  }
  return true;
}

std::unordered_map<std::string, cmdenums> stringtoenums = {
    {"OpenGL", SetRendererAsOpenGL},
    {"openGL", SetRendererAsOpenGL},
    {"GL", SetRendererAsOpenGL},
    {"gl", SetRendererAsOpenGL},
    {"vk", SetRendererAsVulkan},
    {"Vk", SetRendererAsVulkan},
    {"VK", SetRendererAsVulkan},
    {"vulkan", SetRendererAsVulkan},
    {"Vulkan", SetRendererAsVulkan},
    {"Software", SetRendererAsSoftware},
    {"software", SetRendererAsSoftware},
    {"fps", SetFPS},
    {"fov", SetFOV},
    {"vsync", SetVsync},
    {"IP", SetServerIP},
    {"ip", SetServerIP},
    {"ServerIP", SetServerIP},
    {"Server", SetIsServer},
    {"server", SetIsServer},
    {"d", SetIsDebug},
    {"debug", SetIsDebug}};

void cmdprocess() {
  std::string arg;
  while (cmdcurrentinput >> arg) {
    SDL_Log("arg: %s", arg.c_str());
    if (stringtoenums.contains(arg)) {
      cmdenums cmd = stringtoenums[arg];
      switch (cmd) {
        case SetIsDebug: {
          Global->IsDebug = true;
          break;
        }
        case SetRendererAsVulkan: {
          Settings->graphicsmode = Vulkan;
          break;
        }
        case SetRendererAsOpenGL: {
          // move to next arguement
          std::string arg2;
          // checks if next arguement exists and is a number.
          if (!(cmdcurrentinput >> arg2) || !(is_number(arg2))) {
            cmdlines << "Wrong Arguements!(OpenGL version)\n";
            return;
          }
          switch (std::stoi(arg2)) {
            case 1:
              Settings->graphicsmode = OpenGL1;
              break;
            case 3:
              Settings->graphicsmode = OpenGL3;
              break;
            case 4:
              Settings->graphicsmode = OpenGL4;
              break;
            default: {
              cmdlines << "Unsupported OpenGL version!\n";
            }
          }
          break;
        }
        case SetRendererAsSoftware: {
          Settings->graphicsmode = Software;
          break;
        }
        case SetVsync: {
          // move to next arguement
          std::string arg2;
          // checks if next arguement exists and is a number.
          if (!(cmdcurrentinput >> arg2) || !(is_number(arg2))) {
            cmdlines << ("Wrong Arguements!(Vsync)\n");
            return;
          }
          Settings->vsync = std::stoi(arg2);
          break;
        }
        case SetFPS: {
          // move to next arguement
          std::string arg2;
          // checks if next arguement exists and is a number.
          if (!(cmdcurrentinput >> arg2) || !(is_number(arg2))) {
            cmdlines << ("Wrong Arguements!(FPS)\n");
            return;
          }
          Settings->fps = std::stoi(arg2);
          break;
        }
        case SetFOV: {
          // move to next arguement
          std::string arg2;
          // checks if next arguement exists and is a number.
          if (!(cmdcurrentinput >> arg2) || !(is_number(arg2))) {
            cmdlines << ("Wrong Arguements!(FOV)\n");
            return;
          }
          Settings->fov = std::stoi(arg2);
          break;
        }
        case SetServerIP: {
          // IPv4 ServerIP string.
          std::string ServerIP;
          // Server Port uint32_t.
          uint32_t ServerPort = 0;

          // If you are a server return false
          if (Global->IsOnline && IsServer) {
            cmdlines
                << ("Wrong Arguements!(Cannot be Server and have IP input at "
                    "the "
                    "same time)\n");
            return;
          }
          std::string arg2;
          if (!(cmdcurrentinput >> arg2)) {
            cmdlines << ("Arguement doesn't exists!(IP)\n");
            return;
          }
          int j = 0;
          while (arg2[j] != ':') {
            if (arg2[j] == '\0') {
              cmdlines << ("Wrong Arguements!(IP)\n");
              return;
            }
            ServerIP += arg2[j];
            j++;
          }
          j++;
          while (arg2[j] != '\0') {
            int temp = arg2[j] - '0';
            if (temp < 0 || temp > 9) {
              cmdlines << ("Wrong Arguements!(PORT)\n");
              return;
            }
            ServerPort *= 10;
            ServerPort += temp;
            j++;
          }
          ClientConnect(ServerIP, ServerPort);
          break;
        }
        case SetIsServer: {
          // Server Port uint32_t.
          uint32_t ServerPort = 0;
          // Add Server(0) user.
          GlobalNetworkStuff->UserIDs.insert(0);
          // If ServerIP is set then you can't be server.
          if (Global->IsOnline && !IsServer) {
            cmdlines
                << ("Wrong Arguements!(Cannot be Server and have IP input at "
                    "the "
                    "same time)\n");
            return;
          }
          std::string arg2;
          if (!(cmdcurrentinput >> arg2)) {
            cmdlines << ("Wrong Arguements!(ServerPort)\n");
            return;
          }
          int j = 0;
          while (arg2[j] != '\0') {
            int temp = arg2[j] - '0';
            if (temp < 0 || temp > 9) {
              cmdlines << ("Wrong Arguements!(ServerPort)\n");
              return;
            }
            ServerPort *= 10;
            ServerPort += temp;
            j++;
          }
          ServerSetup(ServerPort);
          Global->IsOnline = true;
          IsServer = true;
          break;
        }
      }
    } else {
      cmdlines << "arguement " << "\"" << arg << "\"" << " doesn't exist!\n";
    }
  }
}

void pushcmd() {
  cmdlines << cmdcurrentinput.str() << "\n";
  cmdprocess();
  cmdcurrentinput.clear();
}