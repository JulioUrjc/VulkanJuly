Vulkan examples https://github.com/JulioUrjc/VulkanJuly.git

Last updated: 2019-01-22

Credits
- GLM from https://github.com/g-truc/glm/releases
- GLFW from https://www.glfw.org/download.html
- STB from https://github.com/nothings/stb.git
- Vulkan SDK from https://www.lunarg.com/vulkan-sdk/
- TinyobjLoader from https://github.com/syoyo/tinyobjloader
- Chalet obj from https://sketchfab.com/models/e925320e1d5744d9ae661aeff61e7aef



Primero Windows        - Comunicacion con ventanas 
Segundo Instancia      - Datos dela aplicacion y extensiones
					     Comunicacion app con Vulvan
Tercero Layers         - Control de errores y Message callback (VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
					     
    				     VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: Diagnostic message
    				     VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: Informational message like the creation of a resource
    				     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: Message about behavior that is not necessarily an error, but very likely a bug in your application
    				     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: Message about behavior that is invalid and may cause crashes
     
    				     pMessage: The debug message as a null-terminated string
    				     pObjects: Array of Vulkan object handles related to the message
    				     objectCount: Number of objects in array   
Cuarto physical device - Adquiere las tarjetas graficas que tengamos
						 Will be implicitly destroyed when the VkInstance is destroyed
Quinto logical device  - Variable logica de la GPU fisica (pQueuePriorities between 0.0 and 1.0)

Sexto Surface          - La surface va en la windows ya creada (Antes de seleccionar el device)

Septimo SwapChain      - Infraestructura de buffers, se renderiza antes de mostrar en pantalla (No hay default buffer)
                         Cola de imagenes

                         min/max number of images in swap chain, min/max width and height of images
                         pixel format, color space
                         Available presentation modes


                         Surface format (color depth)
                         Presentation mode (conditions for "swapping" images to the screen)
                         Swap extent (resolution of images in swap chain)

                         VK_PRESENT_MODE_IMMEDIATE_KHR | VK_PRESENT_MODE_FIFO_KHR | VK_PRESENT_MODE_FIFO_RELAXED_KHR | VK_PRESENT_MODE_MAILBOX_KHR
Octavo ImageView       - Wrapper de una imagen con datos 
Noveno PipeLine        - *Input assembler* | Vertex shader | Tessellatior | geometry sshader | *Rasterization* | Fragment shader | *Color blending*
                         Shaders gl_Position - gl_VertexIndex

                         Windows glslangValidator.exe -V shader.vert
                         Linux glslangValidator -V shader.vert

                         ShaderModules


                            VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
                            VK_PRIMITIVE_TOPOLOGY_LINE_LIST: line from every 2 vertices without reuse
                            VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: the end vertex of every line is used as start vertex for the next line
                            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: triangle from every 3 vertices without reuse
                            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: the second and third vertex of every triangle are used as first two vertices of the next triangle

                          viewportState
                          rasterizeInfo ***** Permite poner wireframe, fill, etc *****
                          Multisampling - Muchas muestras en un mismo pixel, mejora aliasing
                          Depth and stencil testing
                          Color blending   ( Mix the old and new value to produce a final color - Combine the old and new value using a bitwise operation)
                          Dynamic state - Te permite desabilitar tamaño del viewport o de la linea para hacerlo en tiempo de draw
                          Pipeline layout - Nos permitirá configurar las variables uniform de los shaders

Decimo RenderPass       - 

                            
                            VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
                            VK_ATTACHMENT_LOAD_OP_CLEAR: Clear the values to a constant at the start
                            VK_ATTACHMENT_LOAD_OP_DONT_CARE: Existing contents are undefined; we don't care about them


                            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: Images used as color attachment
                            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: Images to be presented in the swap chain
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: Images to be used as destination for a memory copy operation

Undecimo FrameBuffer     - Asocia el buffer con las imagenes del swap chain

Doudecimo commandBuffer  - Crea el pool de comandos y el buffer de comandos

                          
                            VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: Hint that command buffers are rerecorded with new commands very often (may change memory allocation behavior)
                            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT: Allow command buffers to be rerecorded individually, without this flag they all have to be reset                       together


                            VK_COMMAND_BUFFER_LEVEL_PRIMARY: Can be submitted to a queue for execution, but cannot be called from other command buffers.
                            VK_COMMAND_BUFFER_LEVEL_SECONDARY: Cannot be submitted directly, but can be called from primary command buffers.

                            The actual vkCmdDraw function is a bit anticlimactic, but it's so simple because of all the information we specified in advance. It has the following parameters, aside from the command buffer:

                                vertexCount: Even though we don't have a vertex buffer, we technically still have 3 vertices to draw.
                                instanceCount: Used for instanced rendering, use 1 if you're not doing that.
                                firstVertex: Used as an offset into the vertex buffer, defines the lowest value of gl_VertexIndex.
                                firstInstance: Used as an offset for instanced rendering, defines the lowest value of gl_InstanceIndex.

Ultimo  mainLoop         -  Coge imagen de swapChain, ejecuta el comando sobre la imagen del frameBuffe, devuelve la imagen al SwapChain para presentarla

                            Synchronization \ Semaphores | Acquiring an image | Submitting the command buffer | Subpass dependencies | Presentation

                            Frames in fligh

    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();


    recreateSwapChain(); Cambiar el tamaño de la ventana