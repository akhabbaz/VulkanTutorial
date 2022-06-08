#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
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
/***************************/

//find appropriate queue for graphics commands
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
class HelloTriangleApplication {
public:
    void run();
private:
    GLFWwindow* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger; 
    // in C++ 11 members can be initialized like this.
    VkPhysicalDevice physicalDevice {VK_NULL_HANDLE};
    // logical device
    VkDevice  device;
    VkQueue graphicsQueue;
    VkSurfaceKHR surface;
    void pickPhysicalDevice(void);
    void initVulkan(void);
    void createSurface(void);
    void createLogicalDevice();
    void setupDebugMessenger(void); 
    void createInstance(void);
    void mainLoop(void);
    void cleanup(void);
};

