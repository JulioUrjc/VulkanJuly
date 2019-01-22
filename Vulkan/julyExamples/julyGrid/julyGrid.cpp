/*
* Vulkan Example - BasicGrid
*
* Copyright (C) 2019 by Julio Martin
*
*/

#include "julyGrid.h"

VulkanFramework::VulkanFramework()
{
  initGeo( &triangle_ );
  initGeo( &grid_ );
  initGeo( &axes_ );

}

/////////////////////////////////////////////////////////////////////////////////////////

VulkanFramework::~VulkanFramework()
{
  // Clean up used Vulkan resources 
  // Note: Inherited destructor cleans up resources stored in base class
  vkDestroyPipeline( device, pipelines_.triangle, nullptr );
  vkDestroyPipeline( device, pipelines_.grid, nullptr );
  vkDestroyPipeline( device, pipelines_.axes, nullptr );

  vkDestroyPipelineLayout( device, pipelineLayout_, nullptr );
  vkDestroyDescriptorSetLayout( device, descriptorSetLayout_, nullptr );

  uniformBuffers_.triangle.destroy();
  uniformBuffers_.grid.destroy();
  uniformBuffers_.axes.destroy();

  destroyGeo( &triangle_ );
  destroyGeo( &grid_ );
  destroyGeo( &axes_ );
}

/////////////////////////////////////////////////////////////////////////////////////////

void VulkanFramework::initGeo( Pollitos* p )
{
  p->device = nullptr;
  p->vertexCount = 0;
  p->indexCount = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

void VulkanFramework::destroyGeo( Pollitos* p )
{
  if ( p->device != nullptr )
  {
    vkDestroyBuffer( p->device, p->vertices.buffer, nullptr );
    vkFreeMemory( p->device, p->vertices.memory, nullptr );
    if ( p->indices.buffer != VK_NULL_HANDLE )
    {
      vkDestroyBuffer( p->device, p->indices.buffer, nullptr );
      vkFreeMemory( p->device, p->indices.memory, nullptr );
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////

// Build separate command buffers for every framebuffer image
// Unlike in OpenGL all rendering commands are recorded once into command buffers that are then resubmitted to the queue
// This allows to generate work upfront and from multiple threads, one of the biggest advantages of Vulkan
void VulkanFramework::buildCommandBuffers()
{
  VkCommandBufferBeginInfo cmdBufInfo = vks::initializers::commandBufferBeginInfo();

  // Set clear values for all framebuffer attachments with loadOp set to clear
  // We use two attachments (color and depth) that are cleared at the start of the subpass and as such we need to set clear values for both
  VkClearValue clearValues[2];
  clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
  clearValues[1].depthStencil = { 1.0f, 0 };

  VkRenderPassBeginInfo renderPassBeginInfo = vks::initializers::renderPassBeginInfo();
  renderPassBeginInfo.renderPass = renderPass;
  renderPassBeginInfo.renderArea.offset.x = 0;
  renderPassBeginInfo.renderArea.offset.y = 0;
  renderPassBeginInfo.renderArea.extent.width = width;
  renderPassBeginInfo.renderArea.extent.height = height;
  renderPassBeginInfo.clearValueCount = 2;
  renderPassBeginInfo.pClearValues = clearValues;

  for ( int32_t i = 0; i < drawCmdBuffers.size(); ++i )
  {
    // Set target frame buffer
    renderPassBeginInfo.framebuffer = frameBuffers[i];

    VK_CHECK_RESULT( vkBeginCommandBuffer( drawCmdBuffers[i], &cmdBufInfo ) );

    // Start the first sub pass specified in our default render pass setup by the base class
    // This will clear the color and depth attachment
    vkCmdBeginRenderPass( drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE );

    // Update dynamic viewport state
    VkViewport viewport = vks::initializers::viewport( (float) width, (float) height, 0.0f, 1.0f );
    vkCmdSetViewport( drawCmdBuffers[i], 0, 1, &viewport );

    // Update dynamic scissor state
    VkRect2D scissor = vks::initializers::rect2D( width, height, 0, 0 );
    vkCmdSetScissor( drawCmdBuffers[i], 0, 1, &scissor );

    //vkCmdSetLineWidth( drawCmdBuffers[i], 5.0f );

    VkDeviceSize offsets[1] = { 0 };

    // Triangle
    vkCmdBindDescriptorSets( drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_, 0, 1, &descriptorSets_.triangle, 0, nullptr );
    vkCmdBindPipeline( drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines_.triangle );
    
    vkCmdBindVertexBuffers( drawCmdBuffers[i], 0, 1, &triangle_.vertices.buffer, offsets );
    vkCmdBindIndexBuffer( drawCmdBuffers[i], triangle_.indices.buffer, 0, VK_INDEX_TYPE_UINT32 );
    vkCmdDrawIndexed( drawCmdBuffers[i], triangle_.indexCount, 1, 0, 0, 1 );

    // Grid 
    vkCmdBindDescriptorSets( drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_, 0, 1, &descriptorSets_.grid, 0, nullptr );
    vkCmdBindPipeline( drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines_.grid );
    
    vkCmdBindVertexBuffers( drawCmdBuffers[i], 0, 1, &grid_.vertices.buffer, offsets );
    vkCmdBindIndexBuffer( drawCmdBuffers[i], grid_.indices.buffer, 0, VK_INDEX_TYPE_UINT32 );
    vkCmdDrawIndexed( drawCmdBuffers[i], grid_.indexCount, 1, 0, 0, 1 );

    // Axes 
    vkCmdBindDescriptorSets( drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_, 0, 1, &descriptorSets_.axes, 0, nullptr );
    vkCmdBindPipeline( drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines_.axes );
    
    vkCmdBindVertexBuffers( drawCmdBuffers[i], 0, 1, &axes_.vertices.buffer, offsets );
    vkCmdBindIndexBuffer( drawCmdBuffers[i], axes_.indices.buffer, 0, VK_INDEX_TYPE_UINT32 );
    vkCmdDrawIndexed( drawCmdBuffers[i], axes_.indexCount, 1, 0, 0, 1 );

    drawUI( drawCmdBuffers[i] );

    vkCmdEndRenderPass( drawCmdBuffers[i] );

    // Ending the render pass will add an implicit barrier transitioning the frame buffer color attachment to 
    // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR for presenting it to the windowing system

    VK_CHECK_RESULT( vkEndCommandBuffer( drawCmdBuffers[i] ) );
  }
}

/////////////////////////////////////////////////////////////////////////////////////////

void VulkanFramework::prepareGrid( const int cellCount, const float cellSize )
{
  const float extent = cellCount * cellSize;
  const float halfExtent = extent * 0.5f;
  const glm::vec3 corner( -halfExtent, 0, -halfExtent );
  
  grid_.device = vulkanDevice->logicalDevice;
  
  std::vector<float> vertexBuffer;
  std::vector<uint32_t> indexBuffer;
  grid_.vertexCount = 0;
  grid_.indexCount = 0;
  
  uint32_t currentIndex = 0;
  
  {
    // Renders lines
    glm::vec3 begin( corner.x, corner.y, corner.z );
    glm::vec3 end( corner.x + extent, corner.y, corner.z );
    // Along X axis
    for ( int i = 0; i < cellCount + 1; ++i )
    {
      vertexBuffer.push_back( begin.x ); vertexBuffer.push_back( begin.y ); vertexBuffer.push_back( begin.z );
      vertexBuffer.push_back( 1.0f ); vertexBuffer.push_back( 1.0f ); vertexBuffer.push_back( 1.0f );
      indexBuffer.push_back( currentIndex ); ++currentIndex;
      ++grid_.vertexCount;
      vertexBuffer.push_back( end.x ); vertexBuffer.push_back( end.y ); vertexBuffer.push_back( end.z );
      vertexBuffer.push_back( 1.0f ); vertexBuffer.push_back( 1.0f ); vertexBuffer.push_back( 1.0f );
      indexBuffer.push_back( currentIndex ); ++currentIndex;
      ++grid_.vertexCount;
      begin.z =( begin.z + cellSize );
      end.z = ( end.z + cellSize );
    }
  
    // Along Z axis
    begin = glm::vec3( corner.x, corner.y, corner.z );
    end = glm::vec3( corner.x, corner.y, corner.z + extent );
    for ( int i = 0; i < cellCount + 1; ++i )
    {
      vertexBuffer.push_back( begin.x ); vertexBuffer.push_back( begin.y ); vertexBuffer.push_back( begin.z );
      vertexBuffer.push_back( 1.0f ); vertexBuffer.push_back( 1.0f ); vertexBuffer.push_back( 1.0f );
      indexBuffer.push_back( currentIndex ); ++currentIndex;
      ++grid_.vertexCount;
      vertexBuffer.push_back( end.x ); vertexBuffer.push_back( end.y ); vertexBuffer.push_back( end.z );
      vertexBuffer.push_back( 1.0f ); vertexBuffer.push_back( 1.0f ); vertexBuffer.push_back( 1.0f );
      indexBuffer.push_back( currentIndex ); ++currentIndex;
      ++grid_.vertexCount;
      begin.x=( begin.x + cellSize );
      end.x=( end.x + cellSize );
    }
  }
  
  grid_.indexCount = indexBuffer.size();

  uint32_t vBufferSize = static_cast<uint32_t>( vertexBuffer.size() ) * sizeof( float );
  uint32_t iBufferSize = static_cast<uint32_t>( indexBuffer.size() ) * sizeof( uint32_t );

  // Use staging buffer to move vertex and index buffer to device local memory
  // Create staging buffers
  vks::Buffer vertexStaging, indexStaging;

  // Vertex buffer
  VK_CHECK_RESULT( vulkanDevice->createBuffer(
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &vertexStaging, vBufferSize, vertexBuffer.data() ) );

  // Index buffer
  VK_CHECK_RESULT( vulkanDevice->createBuffer(
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &indexStaging, iBufferSize, indexBuffer.data() ) );

  // Create device local target buffers
  // Vertex buffer
  VK_CHECK_RESULT( vulkanDevice->createBuffer(
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &grid_.vertices, vBufferSize ) );

  // Index buffer
  VK_CHECK_RESULT( vulkanDevice->createBuffer(
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &grid_.indices, iBufferSize ) );

  // Copy from staging buffers
  VkCommandBuffer copyCmd = VulkanExampleBase::createCommandBuffer( VK_COMMAND_BUFFER_LEVEL_PRIMARY, true );

  VkBufferCopy copyRegion{};

  copyRegion.size = grid_.vertices.size;
  vkCmdCopyBuffer( copyCmd, vertexStaging.buffer, grid_.vertices.buffer, 1, &copyRegion );

  copyRegion.size = grid_.indices.size;
  vkCmdCopyBuffer( copyCmd, indexStaging.buffer, grid_.indices.buffer, 1, &copyRegion );

  VulkanExampleBase::flushCommandBuffer( copyCmd, queue, true );

  // Destroy staging resources
  vkDestroyBuffer( vulkanDevice->logicalDevice, vertexStaging.buffer, nullptr );
  vkFreeMemory( vulkanDevice->logicalDevice, vertexStaging.memory, nullptr );
  vkDestroyBuffer( vulkanDevice->logicalDevice, indexStaging.buffer, nullptr );
  vkFreeMemory( vulkanDevice->logicalDevice, indexStaging.memory, nullptr );
}

/////////////////////////////////////////////////////////////////////////////////////////

void VulkanFramework::prepareAxes()
{
  // Position + Color vertex
  std::vector<float> vertexBuffer = { 0.0f,  0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
                                      1.0f,  0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
                                      0.0f,  0.0f, 0.0f,      0.0f, 1.0f, 0.0f,
                                      0.0f,  1.0f, 0.0f,      0.0f, 1.0f, 0.0f,
                                      0.0f,  0.0f, 0.0f,      0.0f, 0.0f, 1.0f,
                                      0.0f,  0.0f, 1.0f,      0.0f, 0.0f, 1.0f };
  std::vector<uint32_t> indexBuffer = { 0, 1, 2, 3, 4, 5 };
  axes_.vertexCount = 6;
  axes_.indexCount = indexBuffer.size();

  //// Position + Color vertex
  //std::vector<float> vertexBuffer = { 2.0f,  2.0f, 0.0f,      1.0f, 0.0f, 0.0f,
  //                                   -2.0f,  2.0f, 0.0f,      0.0f, 1.0f, 0.0f,
  //                                    0.0f, -2.0f, 0.0f,      0.0f, 0.0f, 1.0f };
  ////std::vector<float> vertexBuffer = { 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f };
  //std::vector<uint32_t> indexBuffer = { 0, 1, 1, 2, 2, 0 };
  //triangle_.vertexCount = 3;
  //triangle_.indexCount = indexBuffer.size();


  uint32_t vBufferSize = static_cast<uint32_t>( vertexBuffer.size() ) * sizeof( float );
  uint32_t iBufferSize = static_cast<uint32_t>( indexBuffer.size() ) * sizeof( uint32_t );

  // Use staging buffer to move vertex and index buffer to device local memory
  // Create staging buffers
  vks::Buffer vertexStaging, indexStaging;

  // Vertex buffer
  VK_CHECK_RESULT( vulkanDevice->createBuffer(
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &vertexStaging, vBufferSize, vertexBuffer.data() ) );

  // Index buffer
  VK_CHECK_RESULT( vulkanDevice->createBuffer(
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &indexStaging, iBufferSize, indexBuffer.data() ) );

  // Create device local target buffers
  // Vertex buffer
  VK_CHECK_RESULT( vulkanDevice->createBuffer(
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &axes_.vertices, vBufferSize ) );

  // Index buffer
  VK_CHECK_RESULT( vulkanDevice->createBuffer(
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &axes_.indices, iBufferSize ) );

  // Copy from staging buffers
  VkCommandBuffer copyCmd = VulkanExampleBase::createCommandBuffer( VK_COMMAND_BUFFER_LEVEL_PRIMARY, true );

  VkBufferCopy copyRegion{};

  copyRegion.size = axes_.vertices.size;
  vkCmdCopyBuffer( copyCmd, vertexStaging.buffer, axes_.vertices.buffer, 1, &copyRegion );

  copyRegion.size = axes_.indices.size;
  vkCmdCopyBuffer( copyCmd, indexStaging.buffer, axes_.indices.buffer, 1, &copyRegion );

  VulkanExampleBase::flushCommandBuffer( copyCmd, queue, true );

  // Destroy staging resources
  vkDestroyBuffer( vulkanDevice->logicalDevice, vertexStaging.buffer, nullptr );
  vkFreeMemory( vulkanDevice->logicalDevice, vertexStaging.memory, nullptr );
  vkDestroyBuffer( vulkanDevice->logicalDevice, indexStaging.buffer, nullptr );
  vkFreeMemory( vulkanDevice->logicalDevice, indexStaging.memory, nullptr );
}

/////////////////////////////////////////////////////////////////////////////////////////

void VulkanFramework::prepareVertices()
{
  // Position + Color vertex
  std::vector<float> vertexBuffer = { 1.0f,  1.0f, 0.0f,      1.0f, 0.0f, 0.0f,
                                     -1.0f,  1.0f, 0.0f,      0.0f, 1.0f, 0.0f,   
                                      0.0f, -1.0f, 0.0f,      0.0f, 0.0f, 1.0f };
  //std::vector<float> vertexBuffer = { 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f };
  std::vector<uint32_t> indexBuffer = { 0, 1, 1, 2, 2, 0 };
  triangle_.vertexCount = 3;
  triangle_.indexCount = indexBuffer.size();

  uint32_t vBufferSize = static_cast<uint32_t>( vertexBuffer.size() ) * sizeof( float );
  uint32_t iBufferSize = static_cast<uint32_t>( indexBuffer.size() ) * sizeof( uint32_t );

  // Use staging buffer to move vertex and index buffer to device local memory
  // Create staging buffers
  vks::Buffer vertexStaging, indexStaging;

  // Vertex buffer
  VK_CHECK_RESULT( vulkanDevice->createBuffer(
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &vertexStaging, vBufferSize, vertexBuffer.data() ) );

  // Index buffer
  VK_CHECK_RESULT( vulkanDevice->createBuffer(
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &indexStaging, iBufferSize, indexBuffer.data() ) );

  // Create device local target buffers
  // Vertex buffer
  VK_CHECK_RESULT( vulkanDevice->createBuffer(
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &triangle_.vertices, vBufferSize ) );

  // Index buffer
  VK_CHECK_RESULT( vulkanDevice->createBuffer(
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &triangle_.indices, iBufferSize ) );

  // Copy from staging buffers
  VkCommandBuffer copyCmd = VulkanExampleBase::createCommandBuffer( VK_COMMAND_BUFFER_LEVEL_PRIMARY, true );

  VkBufferCopy copyRegion{};

  copyRegion.size = triangle_.vertices.size;
  vkCmdCopyBuffer( copyCmd, vertexStaging.buffer, triangle_.vertices.buffer, 1, &copyRegion );

  copyRegion.size = triangle_.indices.size;
  vkCmdCopyBuffer( copyCmd, indexStaging.buffer, triangle_.indices.buffer, 1, &copyRegion );

  VulkanExampleBase::flushCommandBuffer( copyCmd, queue, true );

  // Destroy staging resources
  vkDestroyBuffer( vulkanDevice->logicalDevice, vertexStaging.buffer, nullptr );
  vkFreeMemory( vulkanDevice->logicalDevice, vertexStaging.memory, nullptr );
  vkDestroyBuffer( vulkanDevice->logicalDevice, indexStaging.buffer, nullptr );
  vkFreeMemory( vulkanDevice->logicalDevice, indexStaging.memory, nullptr );
}

/////////////////////////////////////////////////////////////////////////////////////////

void VulkanFramework::setupDescriptorPool()
{
  // Example uses one ubo
  std::vector<VkDescriptorPoolSize> poolSizes =
  {
    vks::initializers::descriptorPoolSize( VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2 ),
  };

  VkDescriptorPoolCreateInfo descriptorPoolInfo =
    vks::initializers::descriptorPoolCreateInfo( poolSizes.size(), poolSizes.data(), 3 );


  VK_CHECK_RESULT( vkCreateDescriptorPool( device, &descriptorPoolInfo, nullptr, &descriptorPool ) );
}

/////////////////////////////////////////////////////////////////////////////////////////

void VulkanFramework::setupDescriptorSetLayout()
{
  std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings =
  {
    // Binding 0 : Vertex shader uniform buffer
    vks::initializers::descriptorSetLayoutBinding( VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                   VK_SHADER_STAGE_VERTEX_BIT, 0 )
  };

  VkDescriptorSetLayoutCreateInfo descriptorLayout =
    vks::initializers::descriptorSetLayoutCreateInfo( setLayoutBindings.data(),
                                                      setLayoutBindings.size() );

  VK_CHECK_RESULT( vkCreateDescriptorSetLayout( device, &descriptorLayout,
                                                nullptr, &descriptorSetLayout_ ) );

  VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo =
    vks::initializers::pipelineLayoutCreateInfo( &descriptorSetLayout_, 1 );

  VK_CHECK_RESULT( vkCreatePipelineLayout( device, &pPipelineLayoutCreateInfo,
                                           nullptr, &pipelineLayout_ ) );
}

/////////////////////////////////////////////////////////////////////////////////////////

void VulkanFramework::setupDescriptorSet()
{
  VkDescriptorSetAllocateInfo allocInfo =
    vks::initializers::descriptorSetAllocateInfo( descriptorPool, &descriptorSetLayout_, 1 );

  std::vector<VkWriteDescriptorSet> writeDescriptorSets;

  // Triangle
  VK_CHECK_RESULT( vkAllocateDescriptorSets( device, &allocInfo, &descriptorSets_.triangle ) );
  writeDescriptorSets.push_back( vks::initializers::writeDescriptorSet(
      descriptorSets_.triangle, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &uniformBuffers_.triangle.descriptor )
  );
  
  vkUpdateDescriptorSets( device, writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr );

  // Grid
  VK_CHECK_RESULT( vkAllocateDescriptorSets( device, &allocInfo, &descriptorSets_.grid ) );
  writeDescriptorSets.push_back( vks::initializers::writeDescriptorSet( 
    descriptorSets_.grid, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &uniformBuffers_.grid.descriptor ) );

  vkUpdateDescriptorSets( device, writeDescriptorSets.size(),
                          writeDescriptorSets.data(), 0, nullptr );

  // Axis
  VK_CHECK_RESULT( vkAllocateDescriptorSets( device, &allocInfo, &descriptorSets_.axes ) );
  writeDescriptorSets.push_back( vks::initializers::writeDescriptorSet(
    descriptorSets_.axes, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &uniformBuffers_.axes.descriptor ) );

  vkUpdateDescriptorSets( device, writeDescriptorSets.size(),
                          writeDescriptorSets.data(), 0, nullptr );
}

/////////////////////////////////////////////////////////////////////////////////////////

void VulkanFramework::preparePipelines()
{
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
    vks::initializers::pipelineInputAssemblyStateCreateInfo(
      VK_PRIMITIVE_TOPOLOGY_LINE_LIST, 0, VK_FALSE );

  VkPipelineRasterizationStateCreateInfo rasterizationState =
    vks::initializers::pipelineRasterizationStateCreateInfo(
      VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0 );
  rasterizationState.lineWidth = 1.0f;

  VkPipelineColorBlendAttachmentState blendAttachmentState =
    vks::initializers::pipelineColorBlendAttachmentState( 0xf, VK_FALSE );

  VkPipelineColorBlendStateCreateInfo colorBlendState =
    vks::initializers::pipelineColorBlendStateCreateInfo( 1, &blendAttachmentState );

  VkPipelineDepthStencilStateCreateInfo depthStencilState =
    vks::initializers::pipelineDepthStencilStateCreateInfo( VK_TRUE, VK_TRUE,
                                                            VK_COMPARE_OP_LESS_OR_EQUAL );

  VkPipelineViewportStateCreateInfo viewportState =
    vks::initializers::pipelineViewportStateCreateInfo( 1, 1 );

  VkPipelineMultisampleStateCreateInfo multisampleState =
    vks::initializers::pipelineMultisampleStateCreateInfo( VK_SAMPLE_COUNT_1_BIT );

  std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT,
                                                      VK_DYNAMIC_STATE_SCISSOR };
  VkPipelineDynamicStateCreateInfo dynamicState =
    vks::initializers::pipelineDynamicStateCreateInfo( dynamicStateEnables.data(),
                                                       dynamicStateEnables.size() );

  VkVertexInputBindingDescription vertexInputBinding =
    vks::initializers::vertexInputBindingDescription( 0, sizeof( Vertex ), VK_VERTEX_INPUT_RATE_VERTEX );

  std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = {
    vks::initializers::vertexInputAttributeDescription( 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 ),						// Location 0: Position		
    vks::initializers::vertexInputAttributeDescription( 0, 1, VK_FORMAT_R32G32B32_SFLOAT, sizeof( float ) * 3 )		// Location 1: Color	
  };

  VkPipelineVertexInputStateCreateInfo vertexInputState =
    vks::initializers::pipelineVertexInputStateCreateInfo();
  vertexInputState.vertexBindingDescriptionCount = 1;
  vertexInputState.pVertexBindingDescriptions = &vertexInputBinding;
  vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>( vertexInputAttributes.size() );
  vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();

  std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages;
  shaderStages[0] = loadShader( getAssetPath() + "shadersJuly/julyGrid/julyGrid.vert.spv",
                                VK_SHADER_STAGE_VERTEX_BIT );
  shaderStages[1] = loadShader( getAssetPath() + "shadersJuly/julyGrid/julyGrid.frag.spv",
                                VK_SHADER_STAGE_FRAGMENT_BIT );

  VkGraphicsPipelineCreateInfo pipelineCreateInfo =
    vks::initializers::pipelineCreateInfo( pipelineLayout_, renderPass );
  pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
  pipelineCreateInfo.pRasterizationState = &rasterizationState;
  pipelineCreateInfo.pColorBlendState = &colorBlendState;
  pipelineCreateInfo.pMultisampleState = &multisampleState;
  pipelineCreateInfo.pViewportState = &viewportState;
  pipelineCreateInfo.pDepthStencilState = &depthStencilState;
  pipelineCreateInfo.pDynamicState = &dynamicState;
  pipelineCreateInfo.stageCount = static_cast<uint32_t>( shaderStages.size() );
  pipelineCreateInfo.pStages = shaderStages.data();
  pipelineCreateInfo.pVertexInputState = &vertexInputState;

  VK_CHECK_RESULT( vkCreateGraphicsPipelines( device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipelines_.triangle ) );
  VK_CHECK_RESULT( vkCreateGraphicsPipelines( device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipelines_.grid ) );
  VK_CHECK_RESULT( vkCreateGraphicsPipelines( device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipelines_.axes ) );
}

/////////////////////////////////////////////////////////////////////////////////////////


void VulkanFramework::prepareUniformBuffers()
{
  // Triangle uniform buffer block
  vulkanDevice->createBuffer(
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &uniformBuffers_.triangle,
    sizeof(uboVS_));
  VK_CHECK_RESULT( uniformBuffers_.triangle.map() );

  // Grid uniform buffer block
  vulkanDevice->createBuffer(
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &uniformBuffers_.grid,
    sizeof( uboVS_ ) );
  VK_CHECK_RESULT( uniformBuffers_.grid.map() );

  // Axes uniform buffer block
  vulkanDevice->createBuffer(
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    &uniformBuffers_.axes,
    sizeof( uboVS_ ) );
  VK_CHECK_RESULT( uniformBuffers_.axes.map() );
  
  updateUniformBuffers();
}

/////////////////////////////////////////////////////////////////////////////////////////

void VulkanFramework::updateUniformBuffers()
{
  // Update matrices
  uboVS_.projectionMatrix = glm::perspective( glm::radians( 60.0f ), (float) width / (float) height, 0.1f, 256.0f );

  uboVS_.viewMatrix = glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0f, 0.0f, zoom ) );

  uboVS_.modelMatrix = glm::mat4( 1.0f );
  uboVS_.modelMatrix = glm::rotate( uboVS_.modelMatrix, glm::radians( rotation.x ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
  uboVS_.modelMatrix = glm::rotate( uboVS_.modelMatrix, glm::radians( rotation.y ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
  uboVS_.modelMatrix = glm::rotate( uboVS_.modelMatrix, glm::radians( rotation.z ), glm::vec3( 0.0f, 0.0f, 1.0f ) );

  uniformBuffers_.triangle.copyTo( &uboVS_, sizeof( uboVS_ ) );
  uniformBuffers_.grid.copyTo( &uboVS_, sizeof( uboVS_ ) );
  uniformBuffers_.axes.copyTo( &uboVS_, sizeof( uboVS_ ) );
}

/////////////////////////////////////////////////////////////////////////////////////////

void VulkanFramework::prepare()
{
  VulkanExampleBase::prepare();
  
  prepareVertices();
  prepareGrid( 20, 1.0f );
  prepareAxes();

  prepareUniformBuffers();
  setupDescriptorSetLayout();
  preparePipelines();
  setupDescriptorPool();
  setupDescriptorSet();
  buildCommandBuffers();
  prepared = true;
}

/////////////////////////////////////////////////////////////////////////////////////////

void VulkanFramework::render()
{
  if ( !prepared )
    return;
  draw();
}

/////////////////////////////////////////////////////////////////////////////////////////

void VulkanFramework::draw()
{
  //FIXME:
  VulkanExampleBase::prepareFrame();

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &drawCmdBuffers[currentBuffer];
  VK_CHECK_RESULT( vkQueueSubmit( queue, 1, &submitInfo, VK_NULL_HANDLE ) );

  VulkanExampleBase::submitFrame();
}

/////////////////////////////////////////////////////////////////////////////////////////

void VulkanFramework::viewChanged()
{
  updateUniformBuffers();
}

/////////////////////////////////////////////////////////////////////////////////////////

//void VulkanFramework::OnUpdateUIOverlay( vks::UIOverlay *overlay )
//{
//  if ( overlay->header( "Settings" ) )
//  {
//    bool mesh = true;
//    overlay->checkBox( "Display Mesh", &mesh );
//    float value = 1.0f;
//    overlay->sliderFloat( "Animation speed", &value, 0.0f, 10.0f );
//  }
//}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


VulkanFramework *vulkanFramework;
int main()
{
  if ( vulkanFramework != NULL )
  {
    vulkanFramework->handleMessages( hWnd, uMsg, wParam, lParam );
  }
  return ( DefWindowProc( hWnd, uMsg, wParam, lParam ) );
}
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow )
{
  for ( size_t i = 0; i < __argc; i++ ) { VulkanFramework::args.push_back( __argv[i] ); };
  vulkanFramework = new VulkanFramework();
  vulkanFramework->initVulkan();
  vulkanFramework->setupWindow( hInstance, WndProc );
  vulkanFramework->prepare();
  vulkanFramework->renderLoop();
  delete( vulkanFramework );
  return 0;
}
