# Examples

* This repository contains an example on how to use the classes ``AspiredVersionsManager`` and ``ServerCore``
* To build the examples follow these steps:
    * Clone the repository in a directory called ``serving``
    * Open the repository in the provided Dev Container
    * Use the ``Makefile`` targets provided to build the ``tensorflow_model_server`` and the examples for custom ``servables``

# General Concepts

* TF serving is a software designed to manage the lifecycle of (AI) models and to provide access to this resource
* The server acts like a look-up table (LUT): A client states a request with a specific key (might be a string, number or even a video frame) and the server returns a value based on the given inference backend
* TF serving calls inference backends ``servables``
    * In general a ``servable`` can be *any* C++ class 
    * The ``servable`` for standard TF models is called ``SavedModelBundle``
    * In the basic example a ``Hashmap`` aka ``std::unordered_map<string, string>`` is the ``servable``
* In the simplest case the inference backend can be a simple LUT which just maps an input value to an output value. This is done in ``tensorflow_serving/servables/hashmap``
* In a more sophisticated case the backend can be a neural network which takes the request data (e.g. a video frame) as an input and generates the corresponding output data (e.g. a clasification)
* TF serving comes with default support for CPU inference of TF models

# Managers and Sources

* The lifecycle of a servable (loading, serving, unloading and version management) is carried out by the ``Manager`` component of TF serving
* The ``AspiredVersionsManager`` is a special manager which provides access to specific versions of ``servables``
* The ``AspiredVersionsManager`` gets the ``servable`` from a ``Loader``
    * The loader for a ``SavedModelBundle`` loads the data from a serialized TF model
    * The loader for a ``Hashmap`` loads the data from a hashmap in CSV representation
* ``Loader``s are populated by a ``Source``
* The ``FileSystemStoragePathSource`` is a special source that monitors a given file system path for new versions
* There is a simplified version of a ``Source`` which is called ``SourceAdapater`` and even more specific ``SimpleLoaderSourceAdapater`` which is a special source explicitely designed to populate ``Loader``s
* To implement a custom servable source ``SimpleLoaderSourceAdapater`` must be subclassed as in ``HashmapSourceAdapter``
* A ``Manager`` is connected to a source through the call to ``ConnectSourceToTarget`` which injects a callback in the source which must be called to populate a servable
* In case of the ``HashmapSourceAdapter`` this can be achieved by calling ``SetAspiredVersions`` which triggers the callback from the manager. Since this callback is executed in a thread it may take a while until the servable is loaded.
* In order to use a servable from the manager one must follow these steps:
    * Provide an empty ``ServableHandle`` typed with the type with which the ``SimpleLoaderSourceAdapater`` template has been typed. In case of the hashmap it is an ``serving::ServableHandel<std::unordered_map<string, string>>``.  **If this type does not match the manager will not return the servable**
    * The servable is identified by the name property given in the ``SetAspiredVersions`` function call
    * A call to ``GetServableHandel`` from the manager will search for a loaded servable with the given ID and fill it into the empty handle
    * The ``ServableHandle`` can be used as a smart pointer to the underlying servable (C++ class)


# Servers

* Server Core wraps an AspiredVersionsManager and provides a function GetServableHandle which forwards the request directly to the AspiredVersionsManager
* The HTTP server however uses this function with a handle to a ``SavedModelBundle`` *hardcoded* so it is unusable for our purpose (see tensorflow_serving/model_servers/http_rest_api_handler.cc line 233)
* One needs to rewrite at least parts of the HTTP server to request a ``ServableHandle`` with the custom type.