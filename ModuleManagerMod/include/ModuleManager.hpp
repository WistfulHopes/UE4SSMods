#pragma once
#include "ModuleInterface.hpp"
#include "NameTypes.hpp"
#include <Unreal/FOutputDevice.hpp>
#include "Containers/Map.hpp"
#include "FString.hpp"
#include "SymbolResolver.hpp"

namespace ModuleManager
{
    using namespace RC::Unreal;

    /**
     * Enumerates reasons for failed module loads.
     */
    enum class EModuleLoadResult
    {
        /** Module loaded successfully. */
        Success,

        /** The specified module file could not be found. */
        FileNotFound,

        /** The specified module file is incompatible with the module system. */
        FileIncompatible,

        /** The operating system failed to load the module file. */
        CouldNotBeLoadedByOS,

        /** Module initialization failed. */
        FailedToInitialize
    };

    /**
     * Implements the module manager.
     *
     * The module manager is used to load and unload modules, as well as to keep track of all of the
     * modules that are currently loaded. You can access this singleton using FModuleManager::Get().
     */
#pragma warning(disable: 4624)
    class __declspec(dllexport) FModuleManager
    {
    public:
        static std::unordered_map<std::string, void*> SymbolMap;
        static bool bInitialized;
        static RC::Function<FModuleManager&()> GetWrapper;
        static SymbolResolver symbol_resolver;

        static bool InitializeWrapper();
        static void RegisterImport(std::string Symbol, void* Addr, bool bOverride = false);
        static void RegisterImport(std::string Symbol, const char* Signature, bool bOverride = false);

        /**
         * Gets the singleton instance of the module manager.
         *
         * @return The module manager instance.
         */
        static FModuleManager& Get();

        /**
         * Adds a module to our list of modules, unless it's already known.
         *
         * @param InModuleName The base name of the module file.  Should not include path, extension or platform/configuration info.  This is just the "name" part of the module file name.  Names should be globally unique.
         * @param InFilename The fully qualified path of the module file.
         */
        void AddModule(const FName InModuleName, const FString InFilename);

        /**
         * Gets the specified module.
         *
         * @param InModuleName Name of the module to return.
         * @return 	The module, or nullptr if the module is not loaded.
         * @see GetModuleChecked, GetModulePtr
         */
        IModuleInterface* GetModule(const FName InModuleName);
        
        /**
         * Checks whether the specified module is currently loaded.
         *
         * This is an O(1) operation.
         *
         * @param InModuleName The base name of the module file.  Should not include path, extension or platform/configuration info.  This is just the "module name" part of the module file name.  Names should be globally unique.
         * @return true if module is currently loaded, false otherwise.
         * @see AbandonModule, LoadModule, LoadModuleWithFailureReason, UnloadModule
         */
        
        bool IsModuleLoaded(const FName InModuleName) const;
        
        /**
         * Loads the specified module.
         *
         * @param InModuleName The base name of the module file.  Should not include path, extension or platform/configuration info.  This is just the "module name" part of the module file name.  Names should be globally unique.
         * @return The loaded module, or nullptr if the load operation failed.
         * @see AbandonModule, IsModuleLoaded, LoadModuleChecked, LoadModulePtr, LoadModuleWithFailureReason, UnloadModule
         */
        IModuleInterface* LoadModule(const FName InModuleName);
        
        /**
         * Loads the specified module and returns a result.
         *
         * @param InModuleName The base name of the module file.  Should not include path, extension or platform/configuration info.  This is just the "module name" part of the module file name.  Names should be globally unique.
         * @param OutFailureReason Will contain the result.
         * @return The loaded module (null if the load operation failed).
         * @see AbandonModule, IsModuleLoaded, LoadModule, LoadModuleChecked, LoadModulePtr, UnloadModule
         */
        IModuleInterface* LoadModuleWithFailureReason( const FName InModuleName, EModuleLoadResult& OutFailureReason );
        
        /**
         * Unloads a specific module
         * NOTE: You can manually unload a module before the normal shutdown occurs with this, but be careful as you may be unloading another
         * module's dependency too early!
         *
         * @param InModuleName The name of the module to unload.  Should not include path, extension or platform/configuration info.  This is just the "module name" part of the module file name.
         * @param bIsShutdown Is this unload module call occurring at shutdown (default = false).
         * @return true if module was unloaded successfully, false otherwise.
         * @see AbandonModule, IsModuleLoaded, LoadModule, LoadModuleWithFailureReason
         */
        bool UnloadModule( const FName InModuleName, bool bIsShutdown = false );

    private:
        /**
         * Information about a single module (may or may not be loaded.)
         */
        class FModuleInfo
        {
        public:
            /** The original file name of the module, without any suffixes added */
            FString OriginalFilename;

            /** File name of this module (.dll file name) */
            FString Filename;

            /** Handle to this module (DLL handle), if it's currently loaded */
            void* Handle;

            /** The module object for this module.  We actually *own* this module, so it's lifetime is controlled by the scope of this shared pointer. */
            std::unique_ptr<IModuleInterface> Module;

            /** True if this module was unloaded at shutdown time, and we never want it to be loaded again */
            bool bWasUnloadedAtShutdown;

            /** True if this module is full loaded and ready to be used */
            TAtomic<bool> bIsReady;
          
            /** Arbitrary number that encodes the load order of this module, so we can shut them down in reverse order. */
            int32 LoadOrder;

            /** Constructor */
            FModuleInfo()
                : Handle(nullptr)
                , bWasUnloadedAtShutdown(false)
                , bIsReady(false)
                , LoadOrder(0)
            { }

            ~FModuleInfo()
            {
            }
        };
        
        /**
         * Information about a single module (may or may not be loaded.)
         */
        class FModuleInfoOld
        {
        public:
            /** The original file name of the module, without any suffixes added */
            FString OriginalFilename;

            /** File name of this module (.dll file name) */
            FString Filename;

            /** Handle to this module (DLL handle), if it's currently loaded */
            void* Handle;

            /** The module object for this module.  We actually *own* this module, so it's lifetime is controlled by the scope of this shared pointer. */
            std::unique_ptr<IModuleInterface> Module;

            /** True if this module was unloaded at shutdown time, and we never want it to be loaded again */
            bool bWasUnloadedAtShutdown;
              
            /** Arbitrary number that encodes the load order of this module, so we can shut them down in reverse order. */
            int32 LoadOrder;

            /** Constructor */
            FModuleInfoOld()
                : Handle(nullptr)
                , bWasUnloadedAtShutdown(false)
                , LoadOrder(0)
            { }

            ~FModuleInfoOld()
            {
            }
        };
        
        typedef std::shared_ptr<FModuleInfo> ModuleInfoPtr;
        typedef std::shared_ptr<FModuleInfoOld> ModuleInfoPtrOld;

        typedef TMap<FName, ModuleInfoPtr> FModuleMap;
        typedef TMap<FName, ModuleInfoPtrOld> FModuleMapOld;
        
        struct ModuleManagerNew {
            uintptr_t vtable = {}; // padding for vtable

            /** Map of all modules.  Maps the case-insensitive module name to information about that module, loaded or not. */
            FModuleMap Modules;
        };
      
        struct ModuleManagerOld {
            char padding[0x50]; // padding

            /** Map of all modules.  Maps the case-insensitive module name to information about that module, loaded or not. */
            FModuleMapOld Modules;
        };

        uint8 Content[160] {};
    
    public:
        void AddModuleToModulesList(const FName InModuleName, ModuleInfoPtr& ModuleInfo);
        void AddModuleToModulesList(const FName InModuleName, ModuleInfoPtrOld& ModuleInfo);

    private:
        /** Thread safe module finding routine. */
        ModuleInfoPtr FindModule(FName InModuleName);
        /** Thread safe module finding routine. */
        ModuleInfoPtrOld FindModuleOld(FName InModuleName);
        
        FORCEINLINE std::shared_ptr<const FModuleInfo> FindModule(
            FName InModuleName) const
        {
            return const_cast<FModuleManager*>(this)->FindModule(InModuleName);
        }
        
        
        FORCEINLINE std::shared_ptr<const FModuleInfoOld> FindModuleOld(
            FName InModuleName) const
        {
            return const_cast<FModuleManager*>(this)->FindModuleOld(InModuleName);
        }
    };
}
