#include "files.h"

#include <SDL3/SDL_dialog.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_timer.h>
#include <ft2build.h>
#include <glad/glad.h>
#include <stdio.h>

#include <filesystem>
#include <glm/gtc/type_ptr.hpp>
#include <set>
#include <sstream>
#include <unordered_map>

#include FT_FREETYPE_H

#include "camera.h"
#include "extern.h"
#include "font.h"
#include "global.h"
#include "inputs.h"
#include "model.h"
#include "network.h"
#include "networkextern.h"
#include "render.h"
#include "settings.h"
#include "ui.h"
#include "update.h"

// IPv4 ServerIP string.
std::string ServerIP;
// Server Port uint32_t.
uint32_t ServerPort;

// Settings Save function
void SaveSettings() {
  // get location of settings file
  std::string location = strcat(Global->pref_path, "/Settings.txt");

  FILE* file = fopen(location.c_str(), "w");
  if (file == NULL) {
    SDL_Log("Failed to save settings!");
    return;
  }

  fprintf(file, "%d %d %d %d %f %f %hu %hu %d", Settings->autorun,
          Settings->fov, Settings->fps, Settings->graphicsmode,
          Settings->mousesensitivity.x, Settings->mousesensitivity.y,
          Settings->resolutionx, Settings->resolutiony, Settings->vsync);

  fclose(file);
  SDL_Log("Saved settings!");
}

// Settings Load function
void LoadSettings() {
  // get location of settings file
  std::string location = strcat(Global->pref_path, "/Settings.txt");

  FILE* file = fopen(location.c_str(), "r");
  if (file == NULL) {
    SDL_Log("Failed to load settings!");
    return;
  }

  fscanf(file, "%d %d %d %d %f %f %hu %hu %d", &Settings->autorun,
         &Settings->fov, &Settings->fps, &Settings->graphicsmode,
         &Settings->mousesensitivity.x, &Settings->mousesensitivity.y,
         &Settings->resolutionx, &Settings->resolutiony, &Settings->vsync);

  fclose(file);
  SDL_Log("Loaded settings!");
}

// renderer free function
void freeRenderer() {
  SDL_DestroyWindow(RendererGlobal->window);

  switch (Settings->graphicsmode) {  // opengl
    case 1: {
      // free textures
      for (auto& [key, value] : RendererGlobal->GLstuff->textures) {
        glDeleteTextures(1, &value);
      }
      SDL_GL_DestroyContext(RendererGlobal->GLstuff->GLContext);

      // delete opengl pointer
      delete (RendererGlobal->GLstuff);
      break;
    }
    default: {  // software
      SDL_DestroyPalette(RendererGlobal->SRstuff->palette);
      SDL_DestroyRenderer(RendererGlobal->SRstuff->renderer);
      SDL_DestroySurface(RendererGlobal->SRstuff->render_target);
      for (auto& [key, value] : RendererGlobal->SRstuff->textures) {
        SDL_DestroySurface(value);
      }
      delete (RendererGlobal->SRstuff);
      break;
    }
  }
}

// PNG load function
bool loadPNG(std::filesystem::path path) {
  SDL_Surface* surface;
  SDL_Log("Texture: %s", path.filename().string().c_str());
  switch (Settings->graphicsmode) {
    case 2: {  // vulkan

      break;
    }
    case 1: {  // opengl
      std::string tempstr = path.filename().string();
      // remove file extension (png)
      for (int i = 0; i < 4; i++) tempstr.pop_back();
      glGenTextures(1, &(RendererGlobal->GLstuff->textures[tempstr]));
      surface = SDL_LoadPNG(path.string().c_str());
      if (surface == NULL) return false;
      // that one magenta color as transparent color
      SDL_SetSurfaceColorKey(surface, true,
                             SDL_MapSurfaceRGB(surface, 255, 0, 255));
      // Set texture format.
      surface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);

      glBindTexture(GL_TEXTURE_2D, RendererGlobal->GLstuff->textures[tempstr]);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
                   GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      SDL_DestroySurface(surface);
      break;
    }
    default: {  // software
      surface = SDL_LoadPNG(path.string().c_str());
      if (surface == NULL) return false;
      surface = SDL_ConvertSurfaceAndColorspace(
          surface, SDL_PIXELFORMAT_INDEX8, RendererGlobal->SRstuff->palette,
          SDL_COLORSPACE_RGB_DEFAULT, 0);
      SDL_SetSurfacePalette(surface, RendererGlobal->SRstuff->palette);
      std::string tempstr = path.filename().string();
      for (int i = 0; i < 4; i++) tempstr.pop_back();
      RendererGlobal->SRstuff->textures[tempstr] = surface;
    }
  }
  return true;
}

bool VulkanInstancething() {
  auto vkGetInstanceProcAddr =
      (PFN_vkGetInstanceProcAddr)SDL_Vulkan_GetVkGetInstanceProcAddr();

  auto const vulkanVersion{
      RendererGlobal->Vulkanstuff->context.enumerateInstanceVersion()};
  SDL_Log("Vulkan version %d.%d", VK_API_VERSION_MAJOR(vulkanVersion),
          VK_API_VERSION_MINOR(vulkanVersion));

  uint32_t extensioncnt = 0;
  const char* const* instance_extensions =
      SDL_Vulkan_GetInstanceExtensions(&extensioncnt);

  if (instance_extensions == NULL) return false;

  uint32_t count_extensions = extensioncnt + 1;
  const char** extensions =
      (const char**)(SDL_malloc(count_extensions * sizeof(const char*)));
  extensions[0] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
  SDL_memcpy(&extensions[1], instance_extensions,
             extensioncnt * sizeof(const char*));

  constexpr vk::ApplicationInfo appInfo{
      .pApplicationName = "Hello Triangle",
      .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
      .pEngineName = "No Engine",
      .engineVersion = VK_MAKE_VERSION(1, 0, 0),
      .apiVersion = vk::ApiVersion14};

  vk::InstanceCreateInfo createInfo{.pApplicationInfo = &appInfo,
                                    .enabledExtensionCount = count_extensions,
                                    .ppEnabledExtensionNames = extensions};

  RendererGlobal->Vulkanstuff->instance =
      vk::raii::Instance(RendererGlobal->Vulkanstuff->context, createInfo);

  SDL_free(extensions);
  return true;
}

std::vector<const char*> requiredDeviceExtension = {
    vk::KHRSwapchainExtensionName};

// https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/00_Setup/03_Physical_devices_and_queue_families.html
bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice) {
  // Check if the physicalDevice supports the Vulkan 1.3 API version
  bool supportsVulkan1_3 =
      physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

  // Check if any of the queue families support graphics operations
  auto queueFamilies = physicalDevice.getQueueFamilyProperties();
  bool supportsGraphics =
      std::ranges::any_of(queueFamilies, [](auto const& qfp) {
        return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
      });

  // Check if all required physicalDevice extensions are available
  auto availableDeviceExtensions =
      physicalDevice.enumerateDeviceExtensionProperties();
  bool supportsAllRequiredExtensions = std::ranges::all_of(
      requiredDeviceExtension,
      [&availableDeviceExtensions](auto const& requiredDeviceExtension) {
        return std::ranges::any_of(
            availableDeviceExtensions,
            [requiredDeviceExtension](auto const& availableDeviceExtension) {
              return strcmp(availableDeviceExtension.extensionName,
                            requiredDeviceExtension) == 0;
            });
      });

  // Check if the physicalDevice supports the required features (shader draw
  // parameters, dynamic rendering and extended dynamic state)
  auto features = physicalDevice.template getFeatures2<
      vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
      vk::PhysicalDeviceVulkan13Features,
      vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
  bool supportsRequiredFeatures =
      features.template get<vk::PhysicalDeviceVulkan11Features>()
          .shaderDrawParameters &&
      features.template get<vk::PhysicalDeviceVulkan13Features>()
          .dynamicRendering &&
      features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>()
          .extendedDynamicState;

  // Return true if the physicalDevice meets all the criteria
  return supportsVulkan1_3 && supportsGraphics &&
         supportsAllRequiredExtensions && supportsRequiredFeatures;
}

bool VulkanPhysicalDevice() {
  auto physicalDevices =
      RendererGlobal->Vulkanstuff->instance.enumeratePhysicalDevices();
  if (physicalDevices.empty()) return false;

  for (auto const& physicalDevice : physicalDevices) {
    if (isDeviceSuitable(physicalDevice)) {
      RendererGlobal->Vulkanstuff->physicalDevice = physicalDevice;
      return true;
    }
  }

  return false;
}

bool VulkanLogicalDevice() {
  std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
      RendererGlobal->Vulkanstuff->physicalDevice.getQueueFamilyProperties();
  auto graphicsQueueFamilyProperty =
      std::ranges::find_if(queueFamilyProperties, [](auto const& qfp) {
        return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) !=
               static_cast<vk::QueueFlags>(0);
      });
  auto graphicsIndex = static_cast<uint32_t>(std::distance(
      queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
  float queuePriority = 0.5f;
  vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
      .queueFamilyIndex = graphicsIndex,
      .queueCount = 1,
      .pQueuePriorities = &queuePriority};

  return false;
}

// renderer initialization code
bool setRenderer() {
  switch (Settings->graphicsmode) {
    case 2: {  // vulkan
      // ADD VALIDATION LAYERS IF YOU EVER LEARN HOW TO USE THEM
      RendererGlobal->Vulkanstuff = new RendererStuff::VulkanRenderer();

      if (!SDL_Vulkan_LoadLibrary(nullptr)) {
        return false;
      }

      RendererGlobal->window = SDL_CreateWindow(
          "Cobbler Engine", Settings->resolutionx, Settings->resolutiony,
          SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

      if (!VulkanInstancething()) return false;

      if (!VulkanPhysicalDevice()) return false;

      if (!VulkanLogicalDevice()) return false;

      break;
    }
    case 1: {  // opengl
      SDL_Surface* surface;
      RendererGlobal->GLstuff = new RendererStuff::OpenGLRenderer();

      // set opengl version to 1.2 (for n64 compatibility. just in case.)
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                          SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
      // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
      // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

      // create opengl window
      RendererGlobal->window = SDL_CreateWindow(
          "Cobbler Engine", Settings->resolutionx, Settings->resolutiony,
          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

      // Create OpenGL context
      RendererGlobal->GLstuff->GLContext =
          SDL_GL_CreateContext(RendererGlobal->window);

      if (!SDL_GL_MakeCurrent(RendererGlobal->window,
                              RendererGlobal->GLstuff->GLContext))
        return false;

      if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) return false;

      // set vsync
      if (!SDL_GL_SetSwapInterval(Settings->vsync ? 1 : 0)) return false;

      // opengl set stuff
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 256.f);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // set texture map
      std::unordered_map<std::string, GLuint> tempmap;
      tempmap.reserve(64);

      RendererGlobal->GLstuff->textures = tempmap;
      // set backface culling
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glFrontFace(GL_CW);
      // SDL_Log("%d", glGetError());

      break;
    }
    default: {  // software
      SDL_Surface* surface;
      std::string basepath = SDL_GetBasePath(), tempstr = basepath;
      std::unordered_map<std::string, SDL_Surface*> tempvector;
      tempvector.reserve(64);

      RendererGlobal->SRstuff = new RendererStuff::SoftwareRenderer();

      RendererGlobal->SRstuff->textures = tempvector;
      tempstr = basepath;
      tempstr.append("/" + Global->GameName + "/res/Color_palette.png");
      surface = SDL_LoadPNG(tempstr.c_str());

      RendererGlobal->SRstuff->palette = SDL_GetSurfacePalette(surface);
      RendererGlobal->window =
          SDL_CreateWindow("Cobbler Engine", Settings->resolutionx,
                           Settings->resolutiony, SDL_WINDOW_RESIZABLE);
      RendererGlobal->SRstuff->renderer =
          SDL_CreateRenderer(RendererGlobal->window, NULL);
      SDL_SetRenderVSync(RendererGlobal->SRstuff->renderer,
                         Settings->vsync ? 1 : SDL_RENDERER_VSYNC_DISABLED);
      SDL_SetRenderTarget(RendererGlobal->SRstuff->renderer, NULL);
      RendererGlobal->SRstuff->render_target = SDL_CreateSurface(
          Settings->resolutionx, Settings->resolutiony, SDL_PIXELFORMAT_INDEX8);
      SDL_SetSurfacePalette(RendererGlobal->SRstuff->render_target,
                            RendererGlobal->SRstuff->palette);

      RendererGlobal->SRstuff->pixelsdepth.resize(Settings->resolutionx *
                                                  Settings->resolutiony);
      break;
    }
  }

  // load all the textures in the textures folder
  std::string basepath = SDL_GetBasePath();
  for (const auto& entry : std::filesystem::directory_iterator(
           basepath + Global->GameName + "/textures/")) {
    if (entry.is_regular_file()) {
      if (!loadPNG(entry.path())) SDL_Log("Texture load fail!");
    }
  }

  return true;
}

// function that checks if string is a number.
// for checking arguements.
bool is_number(const std::string s) {
  for (int i = 0; i < s.size(); i++) {
    if (!std::isdigit(s[i])) return false;
  }
  return true;
}

// enums for arguements.
enum argenums {
  SetRendererAsVulkan,    // Sets Renderer to Vulkan.
  SetRendererAsOpenGL,    // Sets Renderer to OpenGL.
  SetRendererAsSoftware,  // THIS WILL BE REMOVED EVENTUALLY.
  SetFPS,                 // Sets Frame rate.
  SetFOV,                 // Sets field of view.
  SetVsync,               // Sets Vsync
  SetGame,                // Sets the game.
  // SetLogin,               // Login for website. Work In Progress.
  // SetWebsite,             // The actual IP of the website.
  SetServerIP,  // Set the IP of the server as a client.
  SetIsServer   // Set if you are the server or not.
};

// arguements processing.
bool initargs(std::vector<std::string> args) {
  // Create Global, Settings, LocalInputs, and P1PlayerInputs so that the
  // arguements can change them.
  Global = std::make_unique<GlobalClass>();
  if (Global == nullptr) return false;
  Settings = std::make_unique<SettingsClass>();
  if (Settings == nullptr) return false;
  LocalInputs = new Inputs();
  if (LocalInputs == nullptr) return false;
  P1PlayerInputs = new playerinputs();
  if (P1PlayerInputs == nullptr) return false;
  GlobalMapStuff = std::make_unique<GlobalMapClass>();
  if (GlobalMapStuff == nullptr) return false;
  RendererGlobal = std::make_unique<RendererStuff>();
  if (RendererGlobal == nullptr) return false;
  UIGlobalStuff = std::make_unique<UIGlobalClass>();
  if (UIGlobalStuff == nullptr) return false;
  GlobalNetworkStuff = std::make_unique<GlobalNetworkClass>();
  if (GlobalNetworkStuff == nullptr) return false;

  Global->pref_path =
      SDL_GetPrefPath("CobblerEngine", Global->GameName.c_str());

  SDL_Log("%s", SDL_GetError());

  Settings->fov = 90;

  // Create curlpostfield for website.
  // curlpostfield = new PostField();
  // if (curlpostfield == nullptr) return false;

  // Load Settings automatically.
  LoadSettings();

  SDL_Log("Classes initialized");

  // a map with strings as keys and argenums as values.
  // translates arguement strings to enums.
  std::unordered_map<std::string, argenums> stringtoenums = {
      {"-OpenGL", SetRendererAsOpenGL},
      {"-openGL", SetRendererAsOpenGL},
      {"-GL", SetRendererAsOpenGL},
      {"-gl", SetRendererAsOpenGL},
      {"-vk", SetRendererAsVulkan},
      {"-Vk", SetRendererAsVulkan},
      {"-VK", SetRendererAsVulkan},
      {"-vulkan", SetRendererAsVulkan},
      {"-Vulkan", SetRendererAsVulkan},
      {"-Software", SetRendererAsSoftware},
      {"-software", SetRendererAsSoftware},
      {"-fps", SetFPS},
      {"-fov", SetFOV},
      {"-vsync", SetVsync},
      {"-game", SetGame},
      // {"-login", SetLogin},
      // {"-website", SetWebsite},
      {"-IP", SetServerIP},
      {"-ip", SetServerIP},
      {"-ServerIP", SetServerIP},
      {"-Server", SetIsServer},
      {"-server", SetIsServer}};

  for (int i = 0; i < args.size(); i++) {
    if (stringtoenums.contains(args[i])) {
      argenums temp = stringtoenums[args[i]];
      switch (temp) {
        case SetRendererAsVulkan:
          Settings->graphicsmode = 2;
          break;
        case SetRendererAsOpenGL:
          Settings->graphicsmode = 1;
          break;
        case SetRendererAsSoftware:
          Settings->graphicsmode = 0;
          break;
        case SetVsync:
          // move to next arguement
          i++;
          // checks if next arguement exists and is a number.
          if (i >= args.size() || !is_number(args[i])) {
            SDL_Log("Wrong Arguements!(Vsync)");
            return false;
          }
          Settings->vsync = std::stoi(args[i]);
          break;
        case SetFPS:
          // move to next arguement
          i++;
          // checks if next arguement exists and is a number.
          if (i >= args.size() || !is_number(args[i])) {
            SDL_Log("Wrong Arguements!(FPS)");
            return false;
          }
          Settings->fps = std::stoi(args[i]);
          break;
        case SetFOV:
          // move to next arguement
          i++;
          // checks if next arguement exists and is a number.
          if (i >= args.size() || !is_number(args[i])) {
            SDL_Log("Wrong Arguements!(FOV)");
            return false;
          }
          Settings->fov = std::stoi(args[i]);
          break;
        case SetGame:
          // move to next arguement
          i++;
          // checks if next arguement exists.
          if (i >= args.size()) {
            SDL_Log("Wrong Arguements!(Game)");
            return false;
          }
          Global->GameName = args[i];
          break;
        // case SetLogin: {
        //   std::string password;
        //   // move to next arguement
        //   i++;
        //   // checks if next arguement exists.
        //   if (i >= args.size()) {
        //     SDL_Log("Wrong Arguements!(username)");
        //     return false;
        //   }
        //   // sets username for the website.
        //   curlpostfield->username = args[i];
        //   // move to next arguement
        //   i++;
        //   // checks if next arguement exists.
        //   if (i >= args.size()) {
        //     SDL_Log("Wrong Arguements!(password)");
        //     return false;
        //   }
        //   // sets password for the website.
        //   password = args[i];

        //   // set string that you will send as post field for website.
        //   curlloginstring = "IsGame=True&username=" + curlpostfield->username
        //   +
        //                     "&password=" + password;
        //   break;
        // }
        // case SetWebsite:
        //   // you probably know what this does at this point.
        //   i++;
        //   if (i >= args.size()) {
        //     SDL_Log("Wrong Arguements!(Website)");
        //     return false;
        //   }
        //   curlpostfield->websiteaddr = args[i];
        //   break;
        case SetServerIP: {
          // If you are a server return false
          if (IsServer) {
            SDL_Log(
                "Wrong Arguements!(Cannot be Server and have IP input at the "
                "same time)");
            return false;
          }
          i++;
          if (i >= args.size()) {
            SDL_Log("Wrong Arguements!(IP)");
            return false;
          }
          int j = 0;
          while (args[i][j] != ':') {
            if (args[i][j] == '\0') {
              SDL_Log("Wrong Arguements!(IP)");
              return false;
            }
            ServerIP += args[i][j];
            j++;
          }
          j++;
          while (args[i][j] != '\0') {
            int temp = args[i][j] - '0';
            if (temp < 0 || temp > 9) {
              SDL_Log("Wrong Arguements!(PORT)");
              return false;
            }
            ServerPort *= 10;
            ServerPort += temp;
            j++;
          }
          break;
        }
        case SetIsServer:
          // Add Server(0) user.
          GlobalNetworkStuff->UserIDs.insert(0);
          // If ServerIP is set then you can't be server.
          if (ServerIP != "") {
            SDL_Log(
                "Wrong Arguements!(Cannot be Server and have IP input at the "
                "same time)");
            return false;
          }
          i++;
          if (i >= args.size()) {
            SDL_Log("Wrong Arguements!(ServerPort)");
            return false;
          }
          int j = 0;
          while (args[i][j] != '\0') {
            int temp = args[i][j] - '0';
            if (temp < 0 || temp > 9) {
              SDL_Log("Wrong Arguements!(ServerPort)");
              return false;
            }
            ServerPort *= 10;
            ServerPort += temp;
            j++;
          }
          Global->IsOnline = true;
          IsServer = true;
          break;
      }
    }
  }
  SDL_Log("args done");
  return true;
}

// overall initialization function.
bool init() {
  // Your Global is running? You better go catch it.
  Global->IsRunning = true;

  std::shared_ptr<ZipData> LoadedData(new ZipData());

  // read the resources file of the game.
  FILE* file = fopen((Global->GameName + "/resources.txt").c_str(), "r");
  if (file == NULL) {
    SDL_Log("Impossible to open the file!");
    return false;
  }
  // time to read the contents of the file.
  while (true) {
    char lineHeader[128];
    // read the first word of the line
    if (fscanf(file, "%s", lineHeader) == EOF) break;

    if (strcmp(lineHeader, "START") == 0) {  // Starting stage.
      char name[64];
      fscanf(file, "%s\n", name);
      LoadedData->startlevel = name;
    } else if (strcmp(lineHeader, "FONT") == 0) {  // Font.
      char name[64];
      fscanf(file, "%s\n", name);
      LoadedData->fontname = name;
    } else if (strcmp(lineHeader, "STAGE") == 0) {  // Stage.
      char name[64];
      fscanf(file, "%s\n", name);
      LoadedData->stagenames.push_back(name);
    }
  }
  fclose(file);

  SDL_Log("Loaded resources data");

  // initialize SDL
  if (!SDL_SetAppMetadata(Global->GameName.c_str(), "0.1",
                          "com.example.myapp") ||
      !SDL_Init(SDL_INIT_VIDEO))
    return false;
  SDL_Log("SDL initialized");

  // Initialize Network stuff
  if (!CobblerInitNet()) {
    return false;
  }
  SDL_Log("Net Loaded");
  // if (curlloginstring != "") {
  //   // try to log in.
  //   if (!CobblerCurlLogin()) {
  //     SDL_Log("Login failed");
  //     Global->LoggedIn = false;
  //   } else {
  //     SDL_Log("Login successful");
  //     Global->LoggedIn = true;
  //   }
  // }

  // Server Setup.
  if (IsServer) {
    if (!CobblerSetSocket(ServerPort)) {
      SDL_Log("Server Setup failed");
    }
  }

  // Client Setup.
  if (ServerIP != "") {
    if (!CobblerSetSocket(0)) {
      SDL_Log("Server connection failed");
    }
    CobblerAddIP(ServerIP, ServerPort, 0);
    Global->IsOnline = true;
    std::vector<uint8_t> buffer{};
    bool check = false;

    // check if the server acknowledged you
    while (Global->IsRunning && !check) {
      events();
      CobblerQueueData("PlayerAdd", buffer, 0);
      std::vector<CobblerNetData>* tempvector = CobblerRecvNet();
      if (tempvector != NULL) {
        while (!tempvector->empty()) {
          CobblerNetData* tempdata = &tempvector->back();
          SDL_Log("%s", tempdata->name.c_str());
          check = true;
          tempvector->pop_back();
          break;
        }
        delete tempvector;
      }
      CobblerSendNet();
      SDL_DelayNS(1000000000 / 30);
    }
    // SDL_Log("What");
  }

  if (!Global->IsRunning) return false;

  // get the base path of the game.
  std::string basepath = SDL_GetBasePath(), tempstr, namestr;

  // Set the Renderer.
  if (!setRenderer()) return false;

  // capture the mouse!! Get it!!! NOW!!!
  SDL_SetWindowRelativeMouseMode(RendererGlobal->window, true);

  RendererGlobal->windowscale =
      SDL_GetWindowDisplayScale(RendererGlobal->window);

  // set perspective matrix.
  double fovy =
      2.0 * std::atan(std::tan(glm::radians((double)Settings->fov) * 0.5) /
                      (Settings->resolutionx / (double)Settings->resolutiony));
  Global->perspectivematrix = glm::perspective(
      fovy, Settings->resolutionx / (double)Settings->resolutiony, 0.1, 256.0);

  Camera = new CameraClass();

  // read map data.
  Mapdata tempmapdata;

  for (const auto& dir : std::filesystem::directory_iterator(
           basepath + Global->GameName + "/models/")) {
    if (dir.is_directory()) {
      for (const auto& entry : std::filesystem::directory_iterator(
               basepath + Global->GameName + "/map/" + LoadedData->startlevel +
               "/")) {
        // check if file is a .map file.
        if (entry.is_regular_file() && entry.path().extension() == ".map") {
          file = fopen(entry.path().c_str(), "r");
        } else if (entry.is_regular_file() &&
                   entry.path().extension() == ".png") {  // Load Textures.
          if (!loadPNG(entry.path())) SDL_Log("Texture load fail!");
        }
      }
    }
  }

  if (file == NULL) {
    SDL_Log("Impossible to open the file!");
    return false;
  }
  // time to read the contents of the file.
  while (true) {
    char lineHeader[64];
    // read the first word of the line
    if (fscanf(file, "%s", lineHeader) == EOF) break;

    if (strcmp(lineHeader, "P") == 0) {  // Points.
      MapPoint temppoint;
      fscanf(file, "%f,%f,%f %f,%f,%f\n", &temppoint.pos.x, &temppoint.pos.y,
             &temppoint.pos.z, &temppoint.shade[0], &temppoint.shade[1],
             &temppoint.shade[2]);
      tempmapdata.Points.push_back(temppoint);
    } else if (strcmp(lineHeader, "F") == 0) {  // Faces.
      char texture[64];
      Mapface tempface;
      tempface.points.resize(3);
      tempface.UVs.resize(3);
      fscanf(file, "%d %s %d,%d %d,%d,%d %f,%f %f,%f %f,%f\n",
             &tempface.doublesided, texture, &tempface.xloop, &tempface.yloop,
             &tempface.points[0], &tempface.points[1], &tempface.points[2],
             &tempface.UVs[0][0], &tempface.UVs[0][1], &tempface.UVs[1][0],
             &tempface.UVs[1][1], &tempface.UVs[2][0], &tempface.UVs[2][1]);
      tempface.doublesided = true;
      tempface.texture = texture;
      tempmapdata.mapfaces.push_back(tempface);
    } else if (strcmp(lineHeader, "SKYBOX") == 0) {  // Skybox.
      char name[64];
      fscanf(file, "%s\n", name);
      tempmapdata.skybox = name;
    } else if (strcmp(lineHeader, "KP") == 0) {  // Killbox Points.
      glm::vec3 temppoint;
      fscanf(file, "%f,%f,%f\n", &temppoint.x, &temppoint.y, &temppoint.z);
      tempmapdata.KillboxPoints.push_back(temppoint);
    } else if (strcmp(lineHeader, "KF") == 0) {  // Killbox Faces.
      std::vector<int> tempvector(3);
      fscanf(file, "%d,%d,%d\n", &tempvector[0], &tempvector[1],
             &tempvector[2]);
      tempmapdata.KillboxFaces.push_back(tempvector);
    }
  }
  fclose(file);

  GlobalMapStuff->Points = tempmapdata.Points;
  GlobalMapStuff->mapfaces = tempmapdata.mapfaces;
  GlobalMapStuff->skybox = tempmapdata.skybox;

  GlobalMapStuff->KillboxPoints = tempmapdata.KillboxPoints;
  GlobalMapStuff->KillboxFaces = tempmapdata.KillboxFaces;

  SDL_Log("%zu points in map", tempmapdata.Points.size());
  SDL_Log("%zu faces in map", tempmapdata.mapfaces.size());

  // preprocess the faces in the map.
  // turns all quads into triangles.
  // for (int i = 0; i < GlobalMapStuff->mapfaces.size(); i++) {
  //   if (GlobalMapStuff->mapfaces[i].points.size() == 4) {
  //     Mapface temp;
  //     temp.doublesided = GlobalMapStuff->mapfaces[i].doublesided;
  //     temp.xloop = GlobalMapStuff->mapfaces[i].xloop;
  //     temp.yloop = GlobalMapStuff->mapfaces[i].yloop;
  //     temp.texture = GlobalMapStuff->mapfaces[i].texture;
  //     int temppoints[3] = {GlobalMapStuff->mapfaces[i].points[0],
  //                          GlobalMapStuff->mapfaces[i].points[1],
  //                          GlobalMapStuff->mapfaces[i].points[2]};
  //     glm::vec2 tempUV[3] = {GlobalMapStuff->mapfaces[i].UVs[0],
  //                            GlobalMapStuff->mapfaces[i].UVs[1],
  //                            GlobalMapStuff->mapfaces[i].UVs[2]};
  //     temp.points.assign(temppoints, temppoints + 3);
  //     temp.UVs.assign(tempUV, tempUV + 3);
  //     GlobalMapStuff->mapfaces.push_back(temp);
  //     temp.points[0] = GlobalMapStuff->mapfaces[i].points[2];
  //     temp.points[1] = GlobalMapStuff->mapfaces[i].points[3];
  //     temp.points[2] = GlobalMapStuff->mapfaces[i].points[0];
  //     temp.UVs[0] = GlobalMapStuff->mapfaces[i].UVs[2];
  //     temp.UVs[1] = GlobalMapStuff->mapfaces[i].UVs[3];
  //     temp.UVs[2] = GlobalMapStuff->mapfaces[i].UVs[0];
  //     GlobalMapStuff->mapfaces.push_back(temp);
  //     GlobalMapStuff->mapfaces.erase(GlobalMapStuff->mapfaces.begin() + i);
  //     i--;
  //   }
  // }

  if (Settings->graphicsmode == 1) LoadMapGL();

  LocalPlayer = SpawnEntities[Global->playerclass](0, 0);
  LocalPlayer->position.z = 8;
  LocalPlayer->Modelthing->visible = false;
  LocalPlayer->teamindex = -1;
  LocalPlayer->EntityIndex = 0;

  // push LocalPlayer Entity to Entities vector. LocalPlayer Entity will
  // probably always be in index zero, but that doesn't matter since there's a
  // seperate LocalPlayer pointer.
  Entities[0] = LocalPlayer;

  // Spawns all the npcs in the map.
  for (int i = 0; i < tempmapdata.Entities.size(); i++) {
    // SDL_Log("spawned: %s", tempmapdata.Entities[i].name.c_str());
    if (SpawnEntities.contains(tempmapdata.Entities[i].name)) {
      uint32_t index = EntityMapEmptyIndex();
      Entities[index] = SpawnEntities[tempmapdata.Entities[i].name](0, index);
      Entities[index]->position = tempmapdata.Entities[i].pos;
    }
  }

  // set the props.
  Global->Models = tempmapdata.props;

  // get all the files in the models folder.
  for (const auto& dir : std::filesystem::directory_iterator(
           basepath + Global->GameName + "/models/")) {
    if (dir.is_directory()) {
      for (const auto& entry : std::filesystem::directory_iterator(
               basepath + Global->GameName + "/models/" +
               dir.path().filename().string() + "/")) {
        // check if file is a .cbm file.
        if (entry.is_regular_file() && entry.path().extension() == ".cbm") {
          ModelGroupClass modelgroup;
          GlobalClass::Model model;

          tempstr = entry.path().filename().string();
          for (int i = 0; i < 4; i++) tempstr.pop_back();
          namestr = tempstr;
          std::string posename = "default";
          FILE* file = fopen(entry.path().string().c_str(), "r");
          if (file == NULL) {
            SDL_Log("Impossible to open the file !");
            return false;
          }
          // time to read the contents of the file.
          while (true) {
            char lineHeader[128];
            // read the first word of the line
            if (fscanf(file, "%s", lineHeader) == EOF) break;

            if (strcmp(lineHeader, "A") == 0) {  // Animation.
              char name[64];
              uint32_t animend, animstart;
              fscanf(file, "%s %u %u\n", name, &animstart, &animend);
              modelgroup.anim[name][0] = animstart;
              modelgroup.anim[name][1] = animend;
              posename = name;
            } else if (strcmp(lineHeader, "SB") == 0) {  // Static(?) Bone.
              char name[64], parent[64];
              glm::vec3 head, tail;
              ModelGroupClass::Bone::Pose temppose;
              int what = fscanf(
                  file, "%s %f %f %f/%f %f %f/%f %f %f/%f %f %f/%f %f %f %f %s",
                  name, &head.x, &head.y, &head.z, &tail.x, &tail.y, &tail.z,
                  &temppose.pos[0], &temppose.pos[1], &temppose.pos[2],
                  &temppose.scale[0], &temppose.scale[1], &temppose.scale[2],
                  &temppose.rot[3], &temppose.rot[0], &temppose.rot[1],
                  &temppose.rot[2], parent);

              modelgroup.Bonemap[name].parent = parent;
              modelgroup.Bonemap[name].head = head;
              modelgroup.Bonemap[name].tail = tail;
              modelgroup.Bonemap[name].restpose = temppose;

            } else if (strcmp(lineHeader, "FCV") == 0) {  // object visibility
              char name[64];
              char newlinecheck = 'w';
              fscanf(file, "%s\n", name);
              modelgroup.modelvisibility.try_emplace(posename);
              while (newlinecheck != '\n') {
                uint32_t index2;
                float temp;
                fscanf(file, "%u/%f%c", &index2, &temp, &newlinecheck);

                ModelGroupClass::visibilitything tempvisibility;
                tempvisibility.name = name;
                tempvisibility.value[index2] = (temp < 0.5f);
                modelgroup.modelvisibility[posename].push_back(tempvisibility);
              }
            } else if (strcmp(lineHeader, "FC") == 0) {  // Pose value Curves.
              char name[64], thing[64];
              int index;
              char newlinecheck = 'w';
              fscanf(file, "%*48[^\"]\"%48[^\"]\"].%s %d\n ", name, thing,
                     &index);
              modelgroup.Bonemap.try_emplace(name);
              if (strcmp(thing, "location") == 0) {
                while (newlinecheck != '\n') {
                  uint32_t index2;
                  float temp;
                  fscanf(file, "%u/%f%c", &index2, &temp, &newlinecheck);

                  modelgroup.Bonemap[name].Poses[posename].try_emplace(index2);
                  modelgroup.Bonemap[name].Poses[posename][index2].pos[index] =
                      temp;
                }
              } else if (strcmp(thing, "rotation_quaternion") == 0) {
                while (newlinecheck != '\n') {
                  uint32_t index2;
                  float temp;
                  fscanf(file, "%u/%f%c", &index2, &temp, &newlinecheck);
                  modelgroup.Bonemap[name].Poses[posename].try_emplace(index2);
                  modelgroup.Bonemap[name]
                      .Poses[posename][index2]
                      .rot[(index + 3) % 4] = temp;
                }
              } else if (strcmp(thing, "scale") == 0) {
                while (newlinecheck != '\n') {
                  uint32_t index2;
                  float temp;
                  fscanf(file, "%u/%f%c", &index2, &temp, &newlinecheck);
                  modelgroup.Bonemap[name].Poses[posename].try_emplace(index2);
                  modelgroup.Bonemap[name]
                      .Poses[posename][index2]
                      .scale[index] = temp;
                }
              }
            }  // Object from other file.
            else if (strcmp(lineHeader, "L") == 0) {
              char objname[128];
              fscanf(file, "%s", objname);
              modelgroup.Models.push_back(objname);
              SDL_Log("%s", objname);
            } else if (strcmp(lineHeader, "O") == 0) {  // Object.
              if (namestr != tempstr) {
                Global->Modelmap[namestr] = model;

                modelgroup.Models.push_back(namestr);
                SDL_Log("%s", namestr.c_str());
              }
              char objname[128];
              fscanf(file, "%s", objname);
              namestr = objname;
              namestr = tempstr + "/" + namestr;
              model.faces.clear();
              model.points.clear();
              model.texture = "";
            } else if (strcmp(lineHeader, "P") == 0) {  // Points.
              GlobalClass::Model::Vertex vertex;
              char newlinecheck;
              fscanf(file, "%f %f %f%c", &vertex.pos.x, &vertex.pos.y,
                     &vertex.pos.z, &newlinecheck);
              while (newlinecheck != '\n') {
                char name[64];
                float temp;
                fscanf(file, "%s %f%c", name, &temp, &newlinecheck);
                vertex.bone = name;
              }
              model.points.push_back(vertex);
            } else if (strcmp(lineHeader, "F") == 0) {  // Faces.
              GlobalClass::Model::Face face;
              int matches =
                  fscanf(file, "%u %u %u/%f %f %f/%f %f/%f %f/%f %f\n",
                         &face.point[0], &face.point[1], &face.point[2],
                         &face.normal[0], &face.normal[1], &face.normal[2],
                         &face.uv[0].x, &face.uv[0].y, &face.uv[1].x,
                         &face.uv[1].y, &face.uv[2].x, &face.uv[2].y);
              if (matches < 12) {
                SDL_Log("failed to read cbm file face. Matches: %d", matches);
                return false;
              }
              model.faces.push_back(face);
            } else if (strcmp(lineHeader, "T") == 0) {  // Textures.
              char temp[256] = {};
              fscanf(file, "%s\n", temp);
              std::string tempstr2(temp);
              model.texture = tempstr2;
            }
          }
          fclose(file);
          Global->Modelmap[namestr] = model;
          SDL_Log("%s", namestr.c_str());
          modelgroup.Models.push_back(namestr);
          ModelGroupMap[tempstr] = modelgroup;
          SDL_Log("%s", namestr.c_str());
        } else if (entry.is_regular_file() &&
                   entry.path().extension() == ".png") {  // Load Textures.
          if (!loadPNG(entry.path())) SDL_Log("Texture load fail!");
        }
      }
    }
  }

  // Freetype font library load.
  Freetypething = new FreetypeClass();
  tempstr = basepath;

  if (FT_Init_FreeType(&(Freetypething->FTlibrary))) return false;

  tempstr = basepath;
  // load font.
  tempstr.append("/" + Global->GameName + "/res/" + LoadedData->fontname);
  if (FT_New_Face(Freetypething->FTlibrary, tempstr.c_str(), 0,
                  &(Freetypething->FTface)))
    return false;
  FT_Select_Charmap(Freetypething->FTface, ft_encoding_unicode);

  FT_Set_Pixel_Sizes(Freetypething->FTface, 0, 12);

  for (int i = 0; i < 128; i++) {  // preload 128 glyphs.
    FT_UInt glyph_index = FT_Get_Char_Index(Freetypething->FTface, i);
    FT_Load_Glyph(Freetypething->FTface, glyph_index, FT_LOAD_MONOCHROME);
    FT_Render_Glyph(Freetypething->FTface->glyph, FT_RENDER_MODE_MONO);

    Freetypething->Glyphmap[glyph_index] =
        CreateGlyph(Freetypething->FTface->glyph);
  }

  SDL_GetWindowSizeInPixels(RendererGlobal->window, &RendererGlobal->windowx,
                            &RendererGlobal->windowy);

  return true;
}

// Quit function.
void quit() {
  SDL_Log("started quit");
  // free renderer.
  freeRenderer();
  SDL_Log("freed renderer");

  // free inputs.
  delete (LocalInputs);
  delete (P1PlayerInputs);
  SDL_Log("freed LocalInputs");

  // free font related things.
  FT_Done_Face(Freetypething->FTface);
  FT_Done_FreeType(Freetypething->FTlibrary);

  // free all the pixels of Glyphs.
  for (auto& [key, value] : Freetypething->Glyphmap) {
    delete[] (value.pixels);
  }

  delete (Freetypething);

  SDL_Log("freed Freetype stuff");

  // free UI map.
  for (auto& [key, value] : UIGlobalStuff->UImap) {
    while (!value.empty()) {
      delete (value.back());
      value.pop_back();
    }
  }

  // free UI map 3D.
  for (auto& [key, value] : UIGlobalStuff->UImap3D) {
    while (!value.empty()) {
      delete (value.back());
      value.pop_back();
    }
  }

  // free npc Entities and LocalPlayer.
  for (auto& i : Entities) {
    delete (i.second);
  }

  // free particles
  for (auto& i : Particles) {
    delete (i.second);
  }

  // free pref_path
  SDL_free(Global->pref_path);

  // free network stuff.
  CobblerQuitNet();
  SDL_Log("Netfreed");

  // well it's not running anymore I suppose.
  Global->IsRunning = false;
  SDL_Quit();
  SDL_Log("SDL_Quit");
}
