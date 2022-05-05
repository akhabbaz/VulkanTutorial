#include "triangle.h"
/*  Class member functions */
void HelloTriangleApplication::run() {
        initVulkan();
        mainLoop();
        cleanup();
    }
void HelloTriangleApplication::initVulkan(void) {
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

void HelloTriangleApplication::setupDebugMessenger(void){
	if (!enableValidationLayers) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        
       populateDebugMessengerCreateInfo(createInfo);
       if (CreateDebugUtilsMessengerEXT(instance, &createInfo, 
			nullptr, &debugMessenger) != VK_SUCCESS) {
    		throw std::runtime_error("failed to set up debug messenger!");
       }
}

void HelloTriangleApplication::createInstance(void){
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
void HelloTriangleApplication::pickPhysicalDevice(void){
           uint32_t deviceCount = 0;
	   // this could be initialized with the max number of devices the
	   // instance can handle.
	   vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
           if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan"
			"support!");
	   }
	   std::vector<VkPhysicalDevice> devices(deviceCount);
	   // returns VK_SUCCESS
	   VkResult result =
	   	vkEnumeratePhysicalDevices(instance, 
			&deviceCount, devices.data());
	   if (result != VK_SUCCESS) {
		throw std::runtime_error(
			"failed to enumerate physical devices");
           }

	   for (const auto& device :devices){
		if (isDeviceSuitable(device)) {
		 	physicalDevice = device;
			break;
		}
	   }
	   if (physicalDevice == VK_NULL_HANDLE){
		throw std::runtime_error("failed to find a suitable GPU!");
	   }
    }

void HelloTriangleApplication::mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	
}

void HelloTriangleApplication::cleanup(void) {
	if (enableValidationLayers) {
        	DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    	}
        vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
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
//gets glfw required extensions and addes extensions requested for validation
//layers
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
bool requiredExtensionsFound(VkInstanceCreateInfo& createInfo)
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
			return found;
		}
	  }
	  return true;
}
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    
//vkGetInstanceProcAddr gets a pointer to any function in Vulkan.  The
//parentheses cast that pointer which is of type  PFN_vkVoidFunction to the type
//of the function you want.  Somehow auto figures this out.

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, 
			"vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
//destroy function.
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, 
const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    //VK_STRUCTURE_TYPE is a signature for sType field.
	createInfo.sType  = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
				     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
			         VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr; // Optional   
}

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
    void* pUserData) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}


//create a device check function
bool isDeviceSuitable(VkPhysicalDevice device){
	VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	bool deviceSuitable {true};
	deviceSuitable == deviceSuitable && deviceProperties.deviceType ==
		VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && 
		deviceFeatures.geometryShader;
	QueueFamilyIndices indices = findQueueFamilies(device);
	//call isComplet to get has_value called.
        return deviceSuitable && indices.isComplete();
}

//find appropriate queue for graphics commands
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device){
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, 
		&queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(
		queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
		queueFamilies.data());
	uint32_t i {0};
	for (const auto& queueFamily : queueFamilies) {
    		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        		indices.graphicsFamily = i;
    		}
		if (indices.isComplete()){
			 break;
		}
    		i++;
	}


	return indices;
}
