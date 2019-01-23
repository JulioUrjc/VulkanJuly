#ifndef JULY_VULKAN_FRAMEWORK_H_
#define JULY_VULKAN_FRAMEWORK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fstream>
#include <vector>
#include <exception>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>
#include "base/vulkanexamplebase.h"
#include "base/VulkanModel.hpp"
#include "base/VulkanBuffer.hpp"

// Set to "true" to enable Vulkan's validation layers (see vulkandebug.cpp for details)
#define ENABLE_VALIDATION false
// Set to "true" to use staging buffers for uploading vertex and index data to device local memory
// See "prepareVertices" for details on what's staging and on why to use it
#define USE_STAGING true

class VulkanFramework : public VulkanExampleBase
{
public:

  // Vertex layout used in this example
  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 color;
  };

  // Struct to allocate grid info
  struct GridInfo
  {
    VkDevice device;
    vks::Buffer vertices;
    uint32_t vertexCount;
    vks::Buffer indices;
    uint32_t indexCount;
  };


  // Uniform buffer block object
  struct
  {
    vks::Buffer grid;
    vks::Buffer triangle;
    vks::Buffer axes;
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
  void prepareTriangle();

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
  void initGeo( GridInfo* p );
  ///
  void destroyGeo( GridInfo* p );
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
  virtual void OnUpdateUIOverlay( vks::UIOverlay *overlay );


private:

  float runningTime;

  GridInfo triangle_;
  GridInfo grid_;
  GridInfo axes_;

  bool blockGrid_;
  bool showGrid_;
};

#endif

