#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include "helloTriangle.h"
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
//
const std::vector<const char*> validationLayers = {
//This layer has standard Vulkan validation functions
	"VK_LAYER_KHRONOS_validation"
};
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
// check which validation layers are available.


bool checkValidationLayerSupport();
//returns all required extensions including glfw extensions and layers
std::vector<const char*> getRequiredExtensions();

//check that required extensions are actually there.
void requiredExtensionsFound(VkInstanceCreateInfo& createInfo);
class HelloTriangleApplication {
public:
    void run() {
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;
    VkInstance instance;
    void initVulkan() {
            createInstance();
	    if(!glfwInit())
	     {
		throw std::runtime_error("init failed");
	     }
	     glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	     glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	     window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    }
    void createInstance(){
	    if (enableValidationLayers){
		    std::cout<< "Validation Layers enabled" << std::endl;
		    if (!checkValidationLayerSupport()){
			    throw std::runtime_error("validation layers requested, but not available");
		    }
	    }
	    VkApplicationInfo appInfo{};
	    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	    appInfo.pApplicationName = "Hello Triangle";
	    appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
	    appInfo.pEngineName = "No Engine";
	    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	    appInfo.apiVersion = VK_API_VERSION_1_0;

	    VkInstanceCreateInfo createInfo{};
	    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	    createInfo.pApplicationInfo = &appInfo;
	    //use routine below to get required and call back extensions
	    auto extensions = getRequiredExtensions();
	
	    createInfo.enabledExtensionCount =
			static_cast<uint32_t>(extensions.size());
	    createInfo.ppEnabledExtensionNames = extensions.data();
	    if (enableValidationLayers){
		    createInfo.enabledLayerCount = 
			    	static_cast<uint32_t>(validationLayers.size());
		    createInfo.ppEnabledLayerNames = validationLayers.data();
	    } else {
		    createInfo.enabledLayerCount = 0;
	    }
	    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
		    	throw std::runtime_error("failed to create instance!");
	    }
	    // checks if required Extensions are found.
	    requiredExtensionsFound(createInfo);
    }
    void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	
    }

    void cleanup() {
        vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();

    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
// returns true if all the layers requested are available. False otherwise
bool checkValidationLayerSupport() {
	uint32_t layerCount;
// List all of the available layers 
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	
	for (const char* layerName: validationLayers){
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers){
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound){
			return false;
		}
	}		
	return true;
}

std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    //This constructor uses iterators, so the double pointer is the first and
    //the second is the last pointer
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
//	This sets up a debug messenger iwth a callback. The macro below becomes
//	the string literal "VK_EXT_debug_utils" 

        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    std::cout << "Required Extensions:" << std::endl;
    for (const char * oneExt: extensions)
	std::cout << oneExt << '\t';
    std::cout << std::endl;
    return extensions;
}

//check that required extensions are actually there. 
// This routine first lists all available extensions and makes sure that enabled
// extensions are actually there.
void requiredExtensionsFound(VkInstanceCreateInfo& createInfo)
{	    
	   // extensions are all the extensions available on the system.
           // createInfo.ppEnabledExtensionNames stores the names of the
           // extensions enabled.  All those enabled should be available.
	    uint32_t extensionCount = 0;
	    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
				nullptr);
	    std::vector<VkExtensionProperties> extensions(extensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, 
			extensions.data());
	    std::cout << "available extensions:\n";
	    for (const auto& extension: extensions){
		std::cout << '\t' << extension.extensionName << '\n';
	    }
	    // check if required extensions are there:
	   std::cout << createInfo.enabledExtensionCount  << " extensions Required. They are:\n";
	   for (uint32_t i = 0; i < createInfo.enabledExtensionCount; ++i){
		std::cout << '\t' << createInfo.ppEnabledExtensionNames[i] <<
			'\t';
		bool found{};
		for (const auto& extension:extensions){
			if (strcmp(extension.extensionName, createInfo.ppEnabledExtensionNames[i])== 0){
				found = true;
			}
		}
		std::cout << (found? "Found": "Not Found") << std::endl;
		if (!found){
			throw std::runtime_error("Required Extension Not"
						" Found");
		}
	  }
}
