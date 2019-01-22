#ifndef JULY_VULKAN_FRAMEWORK_H_
#define JULY_VULKAN_FRAMEWORK_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <array>
#include "optional.h"
#include <set>
#include <unordered_map>


#include <vulkan/vulkan.h>

class VulkanFramework
{
public:

  // Vertex layout used in this example
  //struct Vertex
  //{
  //  float position[3];
  //  float color[3];
  //};
  struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
      VkVertexInputBindingDescription bindingDescription = {};
      bindingDescription.binding = 0;
      bindingDescription.stride = sizeof(Vertex);
      bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
      std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

      attributeDescriptions[0].binding = 0;
      attributeDescriptions[0].location = 0;
      attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[0].offset = offsetof(Vertex, pos);

      attributeDescriptions[1].binding = 0;
      attributeDescriptions[1].location = 1;
      attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
      attributeDescriptions[1].offset = offsetof(Vertex, color);

      attributeDescriptions[2].binding = 0;
      attributeDescriptions[2].location = 2;
      attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
      attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

      return attributeDescriptions;
    }

    bool operator==(const Vertex& other) const {
      return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
  };

  // Struct to allocate grid info
  struct Pollitos
  {
    VkDevice device;
    std::vector<Vertex> vertices;
    uint32_t vertexCount;
    std::vector<Vertex> indices;
    uint32_t indexCount;
  };


  // Uniform buffer block object
  struct
  {
     std::vector<Vertex> grid;
     std::vector<Vertex> triangle;
     std::vector<Vertex> axes;
  }  uniformBuffers_;


  // This way we can just memcopy the ubo data to the ubo
  // Note: You should use data types that align with the GPU in order to avoid manual padding (vec4, mat4)
  struct
  {
    glm::mat4 projectionMatrix;
    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;
  } uboVS_;

  // The pipeline layout is used by a pipline to access the descriptor sets 
  // It defines interface (without binding any actual data) between the shader stages used by the pipeline and the shader resources
  // A pipeline layout can be shared among multiple pipelines as long as their interfaces match
  VkPipelineLayout pipelineLayout_;

  // Pipelines (often called "pipeline state objects") are used to bake all states that affect a pipeline
  // While in OpenGL every state can be changed at (almost) any time, Vulkan requires to layout the graphics (and compute) pipeline states upfront
  // So for each combination of non-dynamic pipeline states you need a new pipeline (there are a few exceptions to this not discussed here)
  // Even though this adds a new dimension of planing ahead, it's a great opportunity for performance optimizations by the driver
  //VkPipeline pipeline_;
  struct
  {
    VkPipeline triangle;
    VkPipeline grid;
    VkPipeline axes;
  } pipelines_;

  // The descriptor set layout describes the shader binding layout (without actually referencing descriptor)
  // Like the pipeline layout it's pretty much a blueprint and can be used with different descriptor sets as long as their layout matches
  VkDescriptorSetLayout descriptorSetLayout_;

  // The descriptor set stores the resources bound to the binding points in a shader
  // It connects the binding points of the different shaders with the buffers and images used for those bindings
  //VkDescriptorSet descriptorSet_;
  struct
  {
    VkDescriptorSet triangle;
    VkDescriptorSet grid;
    VkDescriptorSet axes;
  } descriptorSets_;

public:

  ///
  VulkanFramework( );

  ///
  ~VulkanFramework( );


public:

  // Build separate command buffers for every framebuffer image
  // Unlike in OpenGL all rendering commands are recorded once into command buffers that are then resubmitted to the queue
  // This allows to generate work upfront and from multiple threads, one of the biggest advantages of Vulkan
  void buildCommandBuffers( );

  // Prepare vertex and index buffers for an indexed triangle
  // Also uploads them to device local memory using staging and initializes vertex input and attribute binding to match the vertex shader
  void prepareVertices();

  ///
  void setupDescriptorPool( );

  ///
  void setupDescriptorSetLayout( );

  ///
  void setupDescriptorSet( );

  ///
  void preparePipelines( );

  // Prepare and initialize uniform buffer containing shader uniforms
  void prepareUniformBuffers( );

  ///
  void updateUniformBuffers( );

  
public:
  
  ///
  void initGeo( Pollitos* p );

  ///
  void destroyGeo( Pollitos* p );

  ///
  void prepareGrid( const int cellCount, const float cellSize );

  void prepareAxes();


public:

  ///
  void draw( );

  ///
  void prepare( );

  ///
  virtual void render( );

  ///
  virtual void viewChanged();


public:

  ///
  //virtual void OnUpdateUIOverlay( vks::UIOverlay *overlay );


private:

  float runningTime;

  Pollitos triangle_;
  Pollitos grid_;
  Pollitos axes_;

};

#endif

