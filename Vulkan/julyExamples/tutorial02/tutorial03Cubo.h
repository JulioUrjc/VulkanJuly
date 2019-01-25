#ifndef HelloTriangleApplication_H_
#define HelloTriangleApplication_H_


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance_, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) { return func(instance_, pCreateInfo, pAllocator, pDebugMessenger);
  }  else { return VK_ERROR_EXTENSION_NOT_PRESENT; }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance_, VkDebugUtilsMessengerEXT debugMessenger_, const VkAllocationCallbacks* pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) { func(instance_, debugMessenger_, pAllocator); }
}

struct QueueFamilyIndices {
  std::experimental::optional<uint32_t> graphicsFamily;
  std::experimental::optional<uint32_t> presentFamily;
  bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};


class HelloTriangleApplication
{

public:
  void run();

private:
  ///// Run /////////
  void initWindow();
  void initVulkan();
  void mainLoop();
  void cleanup();

  ///// Init Vulkan ///////
  void createInstance();
  std::vector<const char*> getRequiredExtensions(); // Auxiliar
  bool checkValidationLayerSupport(); // Auxiliar
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  bool isDeviceSuitable(VkPhysicalDevice device_); // Auxiliar
  bool checkDeviceExtensionSupport(VkPhysicalDevice device_); // Auxiliar
  void createLogicalDevice();
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device_); // Auxiliar
  void createSwapChain();
  void cleanupSwapChain();  // Auxiliar
  void recreateSwapChain(); // Auxiliar
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats); // Auxiliar
  VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes); // Auxiliar
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities); // Auxiliar
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device_); // Auxiliar
  void createImageViews();
  void createRenderPass();
  void createGraphicsPipeline();
  VkShaderModule createShaderModule(const std::vector<char>& code); // Auxiliar
  void createFramebuffers();
  void createCommandPool();
  void createCommandBuffers();
  void createSyncObjects();

  //// Vertex buffer /////
  void createVertexBuffer();
  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
  void createIndexBuffer();

  ////  Draw Frame /////
  void drawFrame();


  //// Statics functions /////
  static std::vector<char> readFile(const std::string& filename);
  static void framebufferResizeCallback(GLFWwindow* window_, int width, int height);
  static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
  static const std::string getAssetPath();

private:
  GLFWwindow* window_;

  VkInstance instance_;
  VkDebugUtilsMessengerEXT debugMessenger_;
  VkSurfaceKHR surface_;

  VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
  VkDevice device_;

  VkQueue graphicsQueue_;
  VkQueue presentQueue_;

  VkSwapchainKHR swapChain_;
  std::vector<VkImage> swapChainImages_;
  VkFormat swapChainImageFormat_;
  VkExtent2D swapChainExtent_;
  std::vector<VkImageView> swapChainImageViews_;
  std::vector<VkFramebuffer> swapChainFramebuffers_;

  VkRenderPass renderPass_;
  VkPipelineLayout pipelineLayout_;
  VkPipeline graphicsPipeline_;

  VkCommandPool commandPool_;

  VkBuffer vertexBuffer_;
  VkDeviceMemory vertexBufferMemory_;
  VkBuffer indexBuffer_;
  VkDeviceMemory indexBufferMemory_;

  std::vector<VkCommandBuffer> commandBuffers_;

  std::vector<VkSemaphore> imageAvailableSemaphores_;
  std::vector<VkSemaphore> renderFinishedSemaphores_;
  std::vector<VkFence> inFlightFences_;
  size_t currentFrame_ = 0;

  bool framebufferResized_ = false;

#ifdef NDEBUG
  const bool enableValidationLayers_ = false;
#else
  const bool enableValidationLayers_ = true;
#endif

};

#endif /*HelloTriangleApplication_H_*/