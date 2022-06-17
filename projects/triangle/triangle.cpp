#include "triangle.h"
/*  Class member functions */
void HelloTriangleApplication::run() {
	initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }
void HelloTriangleApplication::initWindow(){	
	    if(!glfwInit())
	     {
		throw std::runtime_error("init failed");
	     }
	    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}
void HelloTriangleApplication::initVulkan(void) {
            createInstance();
	    setupDebugMessenger();
	    createSurface();
	    pickPhysicalDevice();
	    createLogicalDevice();	    
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
                    // add Debug messenger data here.  This is stored in this
                    // function so just call it again.
		    populateDebugMessengerCreateInfo(debugCreateInfo);
	            // pNext is a type const void*, a pointer to void. The
	            // pointer to debugCreateInfo is cast to a pointer of the
	            // same type.  I think this cast is for clarity. pNext would
	            // hold a linked list of structures.  Here it stores the
	            // VkDebugUtilsMessengerCreateInfoEXT structure to hold what
	            // to look for
                    // This pointer is not needed for getting the callback to
                    // work. Not sure why this is hear at all. The three
                    // commands all allow the callback to work.
		    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)
					&debugCreateInfo; 
		   // createInfo.pNext = (void*)
					&debugCreateInfo;
		    //createInfo.pNext = nullptr; 
	    } else {
		    createInfo.enabledLayerCount = 0;
		    createInfo.pNext  = nullptr;
	    }
             // createInfo gets loaded into the instance
	    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
		    	throw std::runtime_error("failed to create instance!");
	    }
	    // checks if required Extensions are found.
	    if (!requiredExtensionsFound(createInfo)){
		throw std::runtime_error("Required extensions not found!");
	    }
}
// this only requires the instance, the surface and the window
void HelloTriangleApplication::createSurface(void){
	   /* This top code actually works in Windows.  It produces the surface. The 
	    * bottom code works too is more general but uses glfw functions.
	    * The top code is documented under "Presentation on Microsoft
	    * Windows" Vulkan Programming guide*/
#ifdef WIN32
	   VkWin32SurfaceCreateInfoKHR createInfo{};
	   createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			/* book says = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE
				 _INFO_KHR */
	   createInfo.pNext = nullptr;
           createInfo.flags = 0;
	   createInfo.hwnd = glfwGetWin32Window(window);
	   createInfo.hinstance = GetModuleHandle(nullptr);
	   std::cout << "Windows Surface" << std::endl;
	   if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS){
		   throw std::runtime_error("failed to create Windows Surface");
		   } 
#else 
	   std::cout<< "General Surface" << std::endl;
	   // this general one uses a glfw object, not a Vulkan object.
	   if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS){
		   throw std::runtime_error("failed to create Windows Surface");
	   } 
#endif

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
		if (isDeviceSuitable(device, surface)) {
		 	physicalDevice = device;
			break;
		}
	   }
	   if (physicalDevice == VK_NULL_HANDLE){
		throw std::runtime_error("failed to find a suitable GPU!");
	   }
    }
void HelloTriangleApplication::createLogicalDevice(){
        // here we call this again rather than getting a cached version.
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
	//create potentially more than one VkDeviceQueueCreateInfo
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	// this set may be one element or two; it will be sorted from lowest to
	// highest. If the present and graphicsFamily are the same, there will
	// be just one. It doesn't matter much the order. One can test later as
	// whether this index supports presentation and also whether it is a
	// graphicsFamily.  If there is only one, it has to.  If there are two
	// one can test for presentation only.
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), 
					indices.presentFamily.value()};
	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies){
		VkDeviceQueueCreateInfo queueCreateInfo{};
		//zero pNext and flags. Necessary here because otherwise we get a
		// segmentation fault.
		queueCreateInfo.sType 
			= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pNext = nullptr;
		queueCreateInfo.flags = 0;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}
	// so far we are not requiring any features nor are we enabling any.
	VkPhysicalDeviceFeatures deviceFeatures{};
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	//zero pNext and flags
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>
		(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount =
			static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(
			 validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}
	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
		VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}
        //	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0,
        //				&graphicsQueue);
	// notice here that the the queue family index is already correct,
	// because that has already passed in findQueueFamilies.
	// the second index is the queue within that family.  We will take the
	// first queue, index 0. After this the graphics, presentQueue already
	// have their correct index.
	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0,
		&graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0,
		&presentQueue);
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
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
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
	std::cout << "Validation Layers Found and Needed: ";
	for (const char* layerName: validationLayers){
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers){
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				std::cout << layerProperties.layerName <<
					std::endl;
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
    // this function initialized both glfwExtensionCount to the number of
    // nonzero extensions needed and glfwExtensions to this routine.
    // glfwExtensions are allocated by the glfw Function and also freed by that
    // function.  We should not delete this array.
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    //This constructor uses iterators, so the double pointer is the first and
    //the second is the last pointer.  Copy the extensions locally.
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
//	This sets up a debug messenger with a callback. The macro below becomes
//	the string literal "VK_EXT_debug_utils" 

        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    std::cout << "Required Instance Extensions:" << std::endl;
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
	    std::cout << "Number of instance extensions available: "
                       << extensions.size() << std::endl;
	   // In order to print out available extensions
	   for (const auto& extension: extensions){
		std::cout << '\t' << extension.extensionName << '\n';
	    } //*/
	    // check if required extensions are there:
	   switch(createInfo.enabledExtensionCount){
	   case 0:
		std::cout << "No extensions Required" << std::endl;
		break;
	   case 1:
                std::cout << "One extension required. It is:";
                break;
	   default:
		std::cout << "Extensions Required: " <<
			createInfo.enabledExtensionCount<< ". They are:"
                        << std::endl;
                break;
	 }
	   for (uint32_t i = 0; i < createInfo.enabledExtensionCount; ++i){
		std::cout << '\t' << createInfo.ppEnabledExtensionNames[i] <<
			'\t';
		bool found{};
		for (const auto& extension:extensions){
			if (strcmp(extension.extensionName, 
				createInfo.ppEnabledExtensionNames[i])== 0){
				found = true;
			}
		}
		std::cout << (found? " Found": "Not Found") << std::endl;
		if (!found){ // fail with false if not found
			return found;
		}
	  }
	  return true;
}
//in VkDebugUtilsMessengerCreateInfoEXT this fills in the sType, the
//messageSeverity field, the messageType and the callback function
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
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    	const VkAllocationCallbacks* pAllocator, 
	VkDebugUtilsMessengerEXT* pDebugMessenger) {
    
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
void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
		VkDebugUtilsMessengerEXT debugMessenger, 
		const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) 
	vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
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
    if (messageSeverity >= 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
	 std::cerr << "validation layer: " 
				<< pCallbackData->pMessage << std::endl;
    }
    return VK_FALSE;
}


//create a device check function. indices are not returned by this function.
bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface){
	VkPhysicalDeviceProperties deviceProperties{};
        VkPhysicalDeviceFeatures deviceFeatures{};
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	bool deviceSuitable {true};
	deviceSuitable == deviceSuitable && deviceProperties.deviceType ==
		VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && 
		deviceFeatures.geometryShader;
	QueueFamilyIndices indices = findQueueFamilies(device, surface);
	bool extensionsSupported = checkDeviceExtensionSupport(device);
	//call isComplete to get has_value called.
        return deviceSuitable && indices.isComplete() && extensionsSupported;
}

//check if the extensions are supported
bool checkDeviceExtensionSupport(VkPhysicalDevice device){
	uint32_t extensionCount;
 	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
		nullptr);

	std::cout << "Device Extensions Found: " << extensionCount << std::endl; 
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, 
		availableExtensions.data());
	std::set<std::string> requiredExtensions( deviceExtensions.begin(),
			deviceExtensions.end());
	for (const VkExtensionProperties& extension: availableExtensions){
		requiredExtensions.erase(extension.extensionName);
 	}
	return requiredExtensions.empty();
}
//find appropriate queue for graphics commands. This finds the cues, and checks
//the flags to see if they support graphics.  It returns the first successful
//cue.
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, 
			VkSurfaceKHR surface){
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
		//check also if this queue supports Surface
		VkBool32 presentSupport = false;
		//checks if the family index supports the surface. Pass this 
		// the device, the index, and the surface.  Returns true or
		// false as a boolean.
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, 
			&presentSupport);
		if (presentSupport){
			indices.presentFamily = i;
		}
		if (indices.isComplete()){
			 break;
		}
    		i++;
	}


	return indices;
}


