#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <optional> // adds has_value to any type to distinguish case of 
		    // invalid entry or not initialized.

//This adds a version number to use 
#include "triangleConfig.h"
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
bool requiredExtensionsFound(VkInstanceCreateInfo& createInfo);

//Debug functions
/*********************************************************/
// debugCallback with a standard signature.
//VKAPI_ATTR VKAPI_CALL specify the argument order for the function, they say so
//this has the correct signature so Vulkan can call it.
//
// first parameter looks like an enum describing error levels. The higher they
// are the more severe.
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
     void* pUserData);
//create pointer to DebugMessenger function
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    	const VkAllocationCallbacks* pAllocator, 
          VkDebugUtilsMessengerEXT* pDebugMessenger);
//destroy the debugMessenger function 
void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
     VkDebugUtilsMessengerEXT debugMessenger, 
	const VkAllocationCallbacks* pAllocator);


//populate DebugMessengerCReateInfo
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&
createInfo);

//create a device check function
bool isDeviceSuitable(VkPhysicalDevice device);
/************************************************/
/* Queue Family Functions  */
/**************************/
//holds indices into Queue families
struct QueueFamilyIndices{
	std::optional<uint32_t> graphicsFamily;
	//isComplete checks has_value
	bool isComplete(){
		return graphicsFamily.has_value();
	}
};


//find appropriate queue for graphics commands
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
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
    VkDebugUtilsMessengerEXT debugMessenger; 
    // in C++ 11 members can be initialized like this.
    VkPhysicalDevice physicalDevice {VK_NULL_HANDLE};
    void pickPhysicalDevice(void);
    void initVulkan(void) {
            createInstance();
	    setupDebugMessenger();
	    pickPhysicalDevice();	    
	    if(!glfwInit())
	     {
		throw std::runtime_error("init failed");
	     }
	     glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	     glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	     window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    }
    void setupDebugMessenger(){
	if (!enableValidationLayers) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        
       populateDebugMessengerCreateInfo(createInfo);
       if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
    		throw std::runtime_error("failed to set up debug messenger!");
}
 
}
    void createInstance(){
	    if (enableValidationLayers){
		    std::cout<< "Validation Layers enabled" << std::endl;
		    if (!checkValidationLayerSupport()){
			    throw std::runtime_error("validation layers requested, but not available");
		    }
	    }
	    VkApplicationInfo appInfo{};
            //appInfo, createInfo and debugCreateInfo are structs declared here
            //and would be destroyed after createInstance closes.  But These are
            //inputs to vkCreateInstance so information here gets copied to
            //instance a class member that has a longer lifetime.
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
	    // add debugging for instance creation.
	    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	    if (enableValidationLayers){
		    createInfo.enabledLayerCount = 
			    	static_cast<uint32_t>(validationLayers.size());
		    createInfo.ppEnabledLayerNames = validationLayers.data();
                    // add Debug messenger data here
		    populateDebugMessengerCreateInfo(debugCreateInfo);
	            // pNext is a type const void*, a pointer to void. The
	            // pointer to debugCreateInfo is cast to a pointer of the
	            // same type.  I think this cast is for clarity.
		    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)
					&debugCreateInfo; 
	    } else {
		    createInfo.enabledLayerCount = 0;
		    createInfo.pNext  = nullptr;
	    }
	    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
		    	throw std::runtime_error("failed to create instance!");
	    }
	    // checks if required Extensions are found.
	    if (!requiredExtensionsFound(createInfo)){
		throw std::runtime_error("Required extensions not found!");
	    }
    }
    void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	
    }

    void cleanup() {
	if (enableValidationLayers) {
        	DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    	}
        vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();

    }
};

