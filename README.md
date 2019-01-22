# VulkanJuly

_Vulkan examples_

+ [Cloning](#Cloning)
+ [Assets](#Assets)
+ [Author](#Author)
+ [Thank you](#Thankyou)

## <a name="Cloning"></a>Cloning 🛠️

>This repository contains submodules for external dependencies, so when doing a fresh clone you need to clone recursively:

```
git clone --recursive https://github.com/SaschaWillems/Vulkan.git
```
>Existing repositories can be updated manually:

```
git submodule init
git submodule update
```
>The repository contains everything required to compile and build the examples on <img src="./images/windowslogo.png" alt="" height="22px" valign="bottom"> Windows, using Visual Studio 2015 compiler that supports C++11.

## <a name="Assets">Assets 📦

>Many examples require assets from the asset pack that is not part of this repository due to file size. A python script is included to download the asset pack that. Run
```
python download_assets.py
```
### <a name="Author">Author ✒️

* **Julio Martin** - *Initial work and documentation* - [JulioMartin](https://github.com/JulioUrjc)

<!-- También puedes mirar la lista de todos los [contribuyentes](https://github.com/your/project/contributors) quíenes han participado en este proyecto.--> 

### <a name="Thankyou">Thank you 🎁

 <!-- 📢 🍺 🤓 📄 📌 🖇️ 🔧 ⌨️ 🔩 ⚙️ 🚀 📋-->

- GLM from https://github.com/g-truc/glm/releases
- GLFW from https://www.glfw.org/download.html
- STB from https://github.com/nothings/stb.git
- Vulkan SDK from https://www.lunarg.com/vulkan-sdk/
- TinyobjLoader from https://github.com/syoyo/tinyobjloader
- Chalet obj from https://sketchfab.com/models/e925320e1d5744d9ae661aeff61e7aef
- VulkanFramework Sascha Willems https://github.com/SaschaWillems/Vulkan.git

