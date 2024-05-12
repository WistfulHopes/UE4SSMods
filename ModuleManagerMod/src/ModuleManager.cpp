#include <ModuleManager.hpp>
#include <Helpers/String.hpp>
#include <polyhook2/Exceptions/AVehHook.hpp>
#include <SigScanner/SinglePassSigScanner.hpp>
#include <Unreal/UnrealVersion.hpp>

namespace ModuleManager
{
    std::unordered_map<std::string, void*> FModuleManager::SymbolMap;
    bool FModuleManager::bInitialized = false;
    RC::Function<FModuleManager&()> FModuleManager::GetWrapper;
    SymbolResolver FModuleManager::symbol_resolver;

    RC::Function<void(FName, bool)> ProcessNewlyLoadedUObjectsWrapper;
    RC::Function<void()> ProcessNewlyLoadedUObjectsWrapperDeprecated;

    void ProcessNewlyLoadedUObjects(FName Package, bool bCanProcessNewlyLoadedObjects)
    {
        if (Version::IsAbove(4, 22))
            ProcessNewlyLoadedUObjectsWrapper(Package, bCanProcessNewlyLoadedObjects);
        else
            ProcessNewlyLoadedUObjectsWrapperDeprecated();
    }

    bool FModuleManager::InitializeWrapper()
    {
        if (bInitialized) return true;
        bInitialized = true;

        if (symbol_resolver.found_pdb)
        {
            auto get_address = symbol_resolver.ResolveSymbol("?Get@FModuleManager@@SAAEAV1@XZ");
            GetWrapper.assign_address(get_address);
            auto process_address = symbol_resolver.ResolveSymbol("?ProcessNewlyLoadedUObjects@@YAXXZ");
            ProcessNewlyLoadedUObjectsWrapper.assign_address(process_address);
            ProcessNewlyLoadedUObjectsWrapperDeprecated.assign_address(process_address);
        }

        RC::SignatureContainer get_container {
              {{"E8 ?? ?? ?? ?? 48 8D 4D C0 E8 ?? ?? ?? ?? 4C 8B C0"}},
                            [&](const RC::SignatureContainer & self) {
                              const intptr_t address = reinterpret_cast<intptr_t>(self.get_match_address() - 0xA);
                              GetWrapper.assign_address((void*)(*reinterpret_cast<int*>(address) + address + 0x4));
                              return true;
              },
              [](RC::SignatureContainer & self)
              {
                  if (!GetWrapper.get_function_address())
                  {
                      RC::Output::send(STR("Failed to resolve address for 'FModuleManager::Get'."));
                      bInitialized = false;
                  }
              },
        };

        RC::SignatureContainer get_container_kh3 {
              {{"48 8B C4 55 41 54 41 55 41 56 41 57 48 8B EC 48 81 EC 80 00 00 00 48 C7 45 D0 FE FF FF FF"}},
                            [&](const RC::SignatureContainer & self) {
                              GetWrapper.assign_address(self.get_match_address());
                              return true;
              },
              [](RC::SignatureContainer & self)
              {
                  if (!GetWrapper.get_function_address())
                  {
                      RC::Output::send(STR("Failed to resolve address for 'FModuleManager::Get'."));
                      bInitialized = false;
                  }
              },
        };

        RC::SignatureContainer get_container_417 {
              {{"48 81 EC 88 00 00 00 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 58 48 8B 05 ? ? ? ?"}},
                            [&](const RC::SignatureContainer & self) {
                              GetWrapper.assign_address(self.get_match_address());
                              return true;
              },
              [](RC::SignatureContainer & self)
              {
                  if (!GetWrapper.get_function_address())
                  {
                      RC::Output::send(STR("Failed to resolve address for 'FModuleManager::Get'."));
                      bInitialized = false;
                  }
              },
        };
        
        RC::SignatureContainer process_container {
              {{"40 53 48 81 EC A0 00 00 00 0F B6 DA"}},
                            [&](const RC::SignatureContainer & self) {
                              ProcessNewlyLoadedUObjectsWrapper.assign_address(self.get_match_address());
                              return true;
              },
              [](RC::SignatureContainer & self)
              {
                  if (!ProcessNewlyLoadedUObjectsWrapper.get_function_address())
                  {
                      RC::Output::send(STR("Failed to resolve address for 'ProcessNewlyLoadedUObjects'."));
                      bInitialized = false;
                  }
              },
        };
      
        RC::SignatureContainer process_container_kh3 {
              {{"48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 E8 ? ? ? ? E8 ? ? ? ?"}},
                            [&](const RC::SignatureContainer & self) {
                              ProcessNewlyLoadedUObjectsWrapperDeprecated.assign_address(self.get_match_address());
                              return true;
              },
              [](RC::SignatureContainer & self)
              {
                  if (!ProcessNewlyLoadedUObjectsWrapperDeprecated.get_function_address())
                  {
                      RC::Output::send(STR("Failed to resolve address for 'ProcessNewlyLoadedUObjects'."));
                      bInitialized = false;
                  }
              },
        };
      
        RC::SignatureContainer process_container_417 {
              {{"48 89 5C 24 08 57 48 83 EC 20 E8 ? ? ? ? 65 48 8B 04 25 58 00 00 00"}},
                            [&](const RC::SignatureContainer & self) {
                              ProcessNewlyLoadedUObjectsWrapperDeprecated.assign_address(self.get_match_address());
                              return true;
              },
              [](RC::SignatureContainer & self)
              {
                  if (!ProcessNewlyLoadedUObjectsWrapperDeprecated.get_function_address())
                  {
                      RC::Output::send(STR("Failed to resolve address for 'ProcessNewlyLoadedUObjects'."));
                      bInitialized = false;
                  }
              },
        };
      
        std::vector<RC::SignatureContainer> signature_containers;
        if (Version::IsAtLeast(4, 25))
        {
            if (!GetWrapper.get_function_address()) signature_containers.push_back(get_container);
            if (!ProcessNewlyLoadedUObjectsWrapper.get_function_address()) signature_containers.push_back(process_container);
        }
        else
        {
            wchar_t exe_name[1024];
            GetModuleFileNameW(GetModuleHandle(nullptr), exe_name, 1023);
            auto exe_absolute_path = std::filesystem::path{exe_name};
            if (exe_absolute_path.filename() == STR("KINGDOM HEARTS III.exe")) 
            {
                if (!GetWrapper.get_function_address()) signature_containers.push_back(get_container_kh3);
                if (!ProcessNewlyLoadedUObjectsWrapperDeprecated.get_function_address()) signature_containers.push_back(process_container_kh3);
            }
            else
            {
                if (!GetWrapper.get_function_address()) signature_containers.push_back(get_container_417);
                if (!ProcessNewlyLoadedUObjectsWrapperDeprecated.get_function_address())  signature_containers.push_back(process_container_417);
            }
        }
      
        RC::SinglePassScanner::SignatureContainerMap signature_containers_map;

        signature_containers_map.emplace(RC::ScanTarget::MainExe, signature_containers);

        RC::SinglePassScanner::start_scan(signature_containers_map);

        return bInitialized;
    }

    void* ResolveSymbol(const std::string &mangledSymbolName) {
        const auto result = FModuleManager::SymbolMap.find(mangledSymbolName);
        if (result != FModuleManager::SymbolMap.end())
        {
          return result->second;
        }
                
        return FModuleManager::symbol_resolver.ResolveSymbol(mangledSymbolName.c_str());
    }

    void FModuleManager::RegisterImport(std::string Symbol, const char* Signature, bool bOverride)
    {
        if (SymbolMap.contains(Symbol))
        {
            if (!bOverride) return;
            SymbolMap.erase(Symbol);
        }

        RC::SignatureContainer get_container{
            {{Signature}},
                [&](const RC::SignatureContainer & self) {
                RegisterImport(Symbol, self.get_match_address(), bOverride);
                return true;
            },
            [Symbol](RC::SignatureContainer & self) {
                if (!ResolveSymbol(Symbol)) {
                    RC::Output::send(std::format(STR("Failed to resolve symbol {}: Signature not found\n"), RC::to_wstring(Symbol)));
                }
            },
        };
      
        RC::SinglePassScanner::SignatureContainerMap signature_containers = {
            {RC::ScanTarget::MainExe, {get_container}},
        };

        RC::SinglePassScanner::start_scan(signature_containers);
    }
    
    void FModuleManager::RegisterImport(std::string Symbol, void* Addr, bool bOverride)
    {
        if (SymbolMap.contains(Symbol))
        {
            if (!bOverride) return;
            SymbolMap.erase(Symbol);
        }

        SymbolMap.insert(std::pair(Symbol, Addr));
    }
    
    FModuleManager& FModuleManager::Get()
    {
        InitializeWrapper();
        return GetWrapper();
    }

    IModuleInterface* FModuleManager::GetModule(const FName InModuleName)
    {
        if (Version::IsAtLeast(4, 25))
        {
            // Do we even know about this module?
            const ModuleInfoPtr ModuleInfo = FindModule(InModuleName);

            if (!ModuleInfo)
            {
              return nullptr;
            }

            // For loading purpose, the GameThread is allowed to query modules that are not yet ready
            return ModuleInfo->bIsReady ? ModuleInfo->Module.get() : nullptr;
        }
        else
        {
            // Do we even know about this module?
            const ModuleInfoPtrOld ModuleInfo = FindModuleOld(InModuleName);

            return ModuleInfo->Module.get();
        }
    }
    
    bool FModuleManager::IsModuleLoaded(const FName InModuleName) const
    {
        if (Version::IsAtLeast(4, 25))
        {
            // Do we even know about this module?
            if (const std::shared_ptr<const FModuleInfo> ModuleInfoPtr = FindModule(InModuleName))
            {
                const FModuleInfo& ModuleInfo = *ModuleInfoPtr;

                // Only if already loaded
                if(ModuleInfo.Module)
                {
                    // Module is loaded and ready
                    return true;
                }
            }
        }
        else
        {
            // Do we even know about this module?
            if (const std::shared_ptr<const FModuleInfoOld> ModuleInfoPtr = FindModuleOld(InModuleName))
            {
                const FModuleInfoOld& ModuleInfo = *ModuleInfoPtr;

                // Only if already loaded
                if(ModuleInfo.Module)
                {
                    // Module is loaded and ready
                    return true;
                }
            }
        }
      
        // Not loaded, or not fully initialized yet (StartupModule wasn't called)
        return false;
    }
    
    IModuleInterface* FModuleManager::LoadModule( const FName InModuleName )
    {
        EModuleLoadResult FailureReason;
        IModuleInterface* Result = LoadModuleWithFailureReason(InModuleName, FailureReason );

        // This should return a valid pointer only if and only if the module is loaded
        checkSlow((Result != nullptr) == IsModuleLoaded(InModuleName))

        return Result;
    }
    
    //checks if memory page is protected and changes access to PAGE_READWRITE if it is protected
    void UnprotectPageIfNeeded(void* pagePointer) {
        MEMORY_BASIC_INFORMATION memoryInfo;
        VirtualQuery(pagePointer, &memoryInfo, sizeof(memoryInfo));
        if ((memoryInfo.Protect & PAGE_READONLY) > 0) {
            VirtualProtect(pagePointer, 1, PAGE_READWRITE, &memoryInfo.Protect);
        }
    }

    bool ResolveDllImportsInternal(unsigned char* codeBase, PIMAGE_IMPORT_DESCRIPTOR importDesc) {
        bool bFailedToResolve = true;
        for (; importDesc->Name; importDesc++) {
            uintptr_t *thunkRef;
            FARPROC *funcRef;
            if (importDesc->OriginalFirstThunk) {
                thunkRef = (uintptr_t *) (codeBase + importDesc->OriginalFirstThunk);
                funcRef = (FARPROC *) (codeBase + importDesc->FirstThunk);
            } else {
                // no hint table
                thunkRef = (uintptr_t *) (codeBase + importDesc->FirstThunk);
                funcRef = (FARPROC *) (codeBase + importDesc->FirstThunk);
            }
            const char* libraryName = (LPCSTR) (codeBase + importDesc->Name);

            HMODULE libraryHandle = GetModuleHandleA(libraryName);
            if (libraryHandle == nullptr) {
                std::string libraryNameString = libraryName;
                //try to load library only once
                libraryHandle = LoadLibraryA(libraryName);
            }
            
            //iterate all thunk import entries and resolve them
            for (; *thunkRef; thunkRef++, funcRef++) {
                const char* importDescriptor;
                if (IMAGE_SNAP_BY_ORDINAL(*thunkRef)) {
                    importDescriptor = (LPCSTR)IMAGE_ORDINAL(*thunkRef);
                } else {
                    auto thunkData = (PIMAGE_IMPORT_BY_NAME) (codeBase + (*thunkRef));
                    importDescriptor = (LPCSTR)&thunkData->Name;
                }
                UnprotectPageIfNeeded(funcRef);
                if (libraryHandle == nullptr) {
                    //library handle is empty, attempt symbol resolution
                    *funcRef = reinterpret_cast<FARPROC>(ResolveSymbol(importDescriptor));
                } else {
                    //we have a library reference for a given import, so use GetProcAddress
                    *funcRef = GetProcAddress(libraryHandle, importDescriptor);
                }
                if (*funcRef == nullptr) {
                    RC::Output::send(std::format(STR("Failed to resolve import of symbol {} from {}\n"), RC::to_wstring(importDescriptor), RC::to_wstring(libraryName)));
                    bFailedToResolve = false;
                }
            }
        }
        return bFailedToResolve;
    }

    typedef BOOL (WINAPI *DllEntryProc)(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

    IModuleInterface* FModuleManager::LoadModuleWithFailureReason(const FName
        InModuleName, EModuleLoadResult& OutFailureReason)
    {
        IModuleInterface* LoadedModule = nullptr;
        OutFailureReason = EModuleLoadResult::Success;

        if (Version::IsAtLeast(4, 25))
        {
            // Do fast check for existing module, this is the most common case
            ModuleInfoPtr FoundModulePtr = FindModule(InModuleName);

            if (FoundModulePtr)
            {
                LoadedModule = FoundModulePtr->Module.get();

                if (LoadedModule)
                {
                    return LoadedModule;
                }
            }

            if (!FoundModulePtr)
            {
                RC::Output::send(std::format(STR("Module '{}' does not exist\n"), InModuleName.ToString()));

                OutFailureReason = EModuleLoadResult::FailedToInitialize;
                return nullptr;
            }

            // Grab the module info.  This has the file name of the module, as well as other info.
            ModuleInfoPtr ModuleInfo = FoundModulePtr;
            
            if (ModuleInfo->Filename.GetCharTArray().Num() == 0 || !std::filesystem::exists(ModuleInfo->Filename.GetCharArray()))
            {
                RC::Output::send(std::format(STR("Module DLL '{}' does not exist\n"), ModuleInfo->Filename.GetCharArray()));

                OutFailureReason = EModuleLoadResult::FileNotFound;
                return nullptr;
            }

            if (std::filesystem::exists(ModuleInfo->Filename.GetCharArray()))
            {
                ProcessNewlyLoadedUObjects(NAME_None, true);
                ModuleInfo->Handle = LoadLibraryEx(ModuleInfo->Filename.GetCharArray(), nullptr, DONT_RESOLVE_DLL_REFERENCES);
                if (ModuleInfo->Handle != nullptr)
                {
                    auto dosHeader = (PIMAGE_DOS_HEADER)ModuleInfo->Handle;
                    auto pNTHeader = (PIMAGE_NT_HEADERS) ((LONGLONG) dosHeader + dosHeader->e_lfanew);
                    auto importsDir = &pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
                    if (importsDir->Size)
                    {
                        auto baseImp = (PIMAGE_IMPORT_DESCRIPTOR) ((unsigned char*)ModuleInfo->Handle + importsDir->VirtualAddress);
                        UnprotectPageIfNeeded(baseImp);
                        if (ResolveDllImportsInternal((unsigned char*)ModuleInfo->Handle, baseImp))
                        {
                            if (pNTHeader->OptionalHeader.AddressOfEntryPoint != 0) {
                                auto DllEntry = (DllEntryProc)(LPVOID)((unsigned char*)ModuleInfo->Handle + pNTHeader->OptionalHeader.AddressOfEntryPoint);
                                // notify library about attaching to process
                                BOOL successful = (*DllEntry)((HINSTANCE) ModuleInfo->Handle, DLL_PROCESS_ATTACH, nullptr);
                                if (!successful) {
                                    RC::Output::send(std::format(STR("Failed to load module {}. Entry point not found\n"), InModuleName.ToString()));

                                    FreeLibrary((HMODULE)ModuleInfo->Handle);
                                    ModuleInfo->Handle = nullptr;
                                    OutFailureReason = EModuleLoadResult::FailedToInitialize;
                                    return nullptr;
                                }
                            }
                            ProcessNewlyLoadedUObjects(InModuleName, true);

                            FInitializeModuleFunctionPtr InitializeModuleFunctionPtr =
                                (FInitializeModuleFunctionPtr)GetProcAddress(
                                    (HMODULE)ModuleInfo->Handle,
                                    "InitializeModule");

                            if (InitializeModuleFunctionPtr != nullptr)
                            {
                                if (ModuleInfo->Module)
                                {
                                    LoadedModule = ModuleInfo->Module.get();
                                }
                                else
                                {
                                    // Initialize the module!
                                    ModuleInfo->Module = std::unique_ptr<IModuleInterface>(InitializeModuleFunctionPtr());

                                    if ( ModuleInfo->Module )
                                    {
                                        // Startup the module
                                        ModuleInfo->Module->StartupModule();

                                        // It's now ok for other threads to use the module.
                                        ModuleInfo->bIsReady = true;
                                          
                                        // Set the return parameter
                                        LoadedModule = ModuleInfo->Module.get();

                                        RC::Output::send(std::format(STR("Successfully loaded module {}!\n"), InModuleName.ToString()));
                                    }
                                    else
                                    {
                                        RC::Output::send(std::format(STR("Failed to load module {}. InitializeModule returned nullptr\n"), InModuleName.ToString()));
                                        
                                        FreeLibrary((HMODULE)ModuleInfo->Handle);
                                        ModuleInfo->Handle = nullptr;
                                        OutFailureReason = EModuleLoadResult::FailedToInitialize;
                                    }
                                }
                            }
                            else
                            {
                                RC::Output::send(std::format(STR("Failed to load module {}. InitializeModule not found\n"), InModuleName.ToString()));

                                FreeLibrary((HMODULE)ModuleInfo->Handle);
                                ModuleInfo->Handle = nullptr;
                                OutFailureReason = EModuleLoadResult::FailedToInitialize;
                            }
                        }
                        else
                        {
                            RC::Output::send(std::format(STR("Failed to load module {}. Cannot resolve imports of the library\n"), InModuleName.ToString()));

                            FreeLibrary((HMODULE)ModuleInfo->Handle);
                            ModuleInfo->Handle = nullptr;
                            OutFailureReason = EModuleLoadResult::FailedToInitialize;
                        }
                    }
                }
                else
                {
                    RC::Output::send(std::format(STR("Failed to load module {}. File {} could not be loaded by OS, GetLastError={}\n"), InModuleName.ToString(), ModuleInfo->Filename.GetCharArray(), GetLastError()));

                    OutFailureReason = EModuleLoadResult::CouldNotBeLoadedByOS;
                }
            }
            else
            {
                RC::Output::send(std::format(STR("Failed to load module {}. Could not find file {}\n"), InModuleName.ToString(), ModuleInfo->Filename.GetCharArray()));

                OutFailureReason = EModuleLoadResult::CouldNotBeLoadedByOS;
            }

            return LoadedModule;
        }
        else
        {
            // Do fast check for existing module, this is the most common case
            ModuleInfoPtrOld FoundModulePtr = FindModuleOld(InModuleName);

            if (FoundModulePtr)
            {
                LoadedModule = FoundModulePtr->Module.get();

                if (LoadedModule)
                {
                    return LoadedModule;
                }
            }

            if (!FoundModulePtr)
            {
                RC::Output::send(std::format(STR("Module '{}' does not exist\n"), InModuleName.ToString()));

                OutFailureReason = EModuleLoadResult::FailedToInitialize;
                return nullptr;
            }

            // Grab the module info.  This has the file name of the module, as well as other info.
            ModuleInfoPtrOld ModuleInfo = FoundModulePtr;
            
            if (ModuleInfo->Filename.GetCharTArray().Num() == 0 || !std::filesystem::exists(ModuleInfo->Filename.GetCharArray()))
            {
                RC::Output::send(std::format(STR("Module DLL '{}' does not exist\n"), ModuleInfo->Filename.GetCharArray()));

                OutFailureReason = EModuleLoadResult::FileNotFound;
                return nullptr;
            }

            if (std::filesystem::exists(ModuleInfo->Filename.GetCharArray()))
            {
                ProcessNewlyLoadedUObjects(NAME_None, true);
                ModuleInfo->Handle = LoadLibraryEx(ModuleInfo->Filename.GetCharArray(), nullptr, DONT_RESOLVE_DLL_REFERENCES);
                if (ModuleInfo->Handle != nullptr)
                {
                    auto dosHeader = (PIMAGE_DOS_HEADER)ModuleInfo->Handle;
                    auto pNTHeader = (PIMAGE_NT_HEADERS) ((LONGLONG) dosHeader + dosHeader->e_lfanew);
                    auto importsDir = &pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
                    if (importsDir->Size)
                    {
                        auto baseImp = (PIMAGE_IMPORT_DESCRIPTOR) ((unsigned char*)ModuleInfo->Handle + importsDir->VirtualAddress);
                        UnprotectPageIfNeeded(baseImp);
                        if (ResolveDllImportsInternal((unsigned char*)ModuleInfo->Handle, baseImp))
                        {
                            if (pNTHeader->OptionalHeader.AddressOfEntryPoint != 0) {
                                auto DllEntry = (DllEntryProc)(LPVOID)((unsigned char*)ModuleInfo->Handle + pNTHeader->OptionalHeader.AddressOfEntryPoint);
                                // notify library about attaching to process
                                BOOL successful = (*DllEntry)((HINSTANCE) ModuleInfo->Handle, DLL_PROCESS_ATTACH, nullptr);
                                if (!successful) {
                                    RC::Output::send(std::format(STR("Failed to load module {}. Entry point not found\n"), InModuleName.ToString()));

                                    FreeLibrary((HMODULE)ModuleInfo->Handle);
                                    ModuleInfo->Handle = nullptr;
                                    OutFailureReason = EModuleLoadResult::FailedToInitialize;
                                    return nullptr;
                                }
                            }
                            ProcessNewlyLoadedUObjects(InModuleName, true);
                          
                            FInitializeModuleFunctionPtr InitializeModuleFunctionPtr =
                                (FInitializeModuleFunctionPtr)GetProcAddress(
                                    (HMODULE)ModuleInfo->Handle,
                                    "InitializeModule");

                            if (InitializeModuleFunctionPtr != nullptr)
                            {
                                if (ModuleInfo->Module)
                                {
                                    LoadedModule = ModuleInfo->Module.get();
                                }
                                else
                                {
                                    // Initialize the module!
                                    ModuleInfo->Module = std::unique_ptr<IModuleInterface>(InitializeModuleFunctionPtr());

                                    if ( ModuleInfo->Module )
                                    {
                                        // Startup the module
                                        ModuleInfo->Module->StartupModule();
                                          
                                        // Set the return parameter
                                        LoadedModule = ModuleInfo->Module.get();

                                        RC::Output::send(std::format(STR("Successfully loaded module {}!\n"), InModuleName.ToString()));
                                    }
                                    else
                                    {
                                        RC::Output::send(std::format(STR("Failed to load module {}. InitializeModule returned nullptr\n"), InModuleName.ToString()));
                                        
                                        FreeLibrary((HMODULE)ModuleInfo->Handle);
                                        ModuleInfo->Handle = nullptr;
                                        OutFailureReason = EModuleLoadResult::FailedToInitialize;
                                    }
                                }
                            }
                            else
                            {
                                RC::Output::send(std::format(STR("Failed to load module {}. InitializeModule not found\n"), InModuleName.ToString()));

                                FreeLibrary((HMODULE)ModuleInfo->Handle);
                                ModuleInfo->Handle = nullptr;
                                OutFailureReason = EModuleLoadResult::FailedToInitialize;
                            }
                        }
                        else
                        {
                            RC::Output::send(std::format(STR("Failed to load module {}. Cannot resolve imports of the library\n"), InModuleName.ToString()));

                            FreeLibrary((HMODULE)ModuleInfo->Handle);
                            ModuleInfo->Handle = nullptr;
                            OutFailureReason = EModuleLoadResult::FailedToInitialize;
                        }
                    }
                }
                else
                {
                    RC::Output::send(std::format(STR("Failed to load module {}. File {} could not be loaded by OS, GetLastError={}\n"), InModuleName.ToString(), ModuleInfo->Filename.GetCharArray(), GetLastError()));

                    OutFailureReason = EModuleLoadResult::CouldNotBeLoadedByOS;
                }
            }
            else
            {
                RC::Output::send(std::format(STR("Failed to load module {}. Could not find file {}\n"), InModuleName.ToString(), ModuleInfo->Filename.GetCharArray()));

                OutFailureReason = EModuleLoadResult::CouldNotBeLoadedByOS;
            }

            return LoadedModule;
        }
    }

    bool FModuleManager::UnloadModule(const FName InModuleName, bool bIsShutdown)
    {
        if (Version::IsAtLeast(4, 25)) {
            // Do we even know about this module?
            ModuleInfoPtr ModuleInfoPtr = FindModule(InModuleName);
            if( ModuleInfoPtr )
            {
                FModuleInfo& ModuleInfo = *ModuleInfoPtr;

                // Only if already loaded
                if( ModuleInfo.Module )
                {
                    // Will offer use-before-ready protection at next reload
                    ModuleInfo.bIsReady = false;

                    // Shutdown the module
                    ModuleInfo.Module->ShutdownModule();

                    // Release reference to module interface.  This will actually destroy the module object.
                    ModuleInfo.Module.release();

                    if( ModuleInfo.Handle != nullptr )
                    {
                        // If we're shutting down then don't bother actually unloading the DLL.  We'll simply abandon it in memory
                        // instead.  This makes it much less likely that code will be unloaded that could still be called by
                        // another module, such as a destructor or other virtual function.  The module will still be unloaded by
                        // the operating system when the process exits.
                        if( !bIsShutdown )
                        {
                            // Unload the DLL
                            FreeLibrary((HMODULE)ModuleInfo.Handle);
                        }
                        ModuleInfo.Handle = nullptr;
                    }

                    // If we're shutting down, then we never want this module to be "resurrected" in this session.
                    // It's gone for good!  So we'll mark it as such so that we can catch cases where a routine is
                    // trying to load a module that we've unloaded/abandoned at shutdown.
                    if( bIsShutdown )
                    {
                        ModuleInfo.bWasUnloadedAtShutdown = true;
                    }

                    return true;
                }
            }
        }
        else
        {
            // Do we even know about this module?
            ModuleInfoPtrOld ModuleInfoPtr = FindModuleOld(InModuleName);
            if( ModuleInfoPtr )
            {
                FModuleInfoOld& ModuleInfo = *ModuleInfoPtr;

                // Only if already loaded
                if( ModuleInfo.Module )
                {
                    // Shutdown the module
                    ModuleInfo.Module->ShutdownModule();

                    // Release reference to module interface.  This will actually destroy the module object.
                    ModuleInfo.Module.release();

                    if( ModuleInfo.Handle != nullptr )
                    {
                        // If we're shutting down then don't bother actually unloading the DLL.  We'll simply abandon it in memory
                        // instead.  This makes it much less likely that code will be unloaded that could still be called by
                        // another module, such as a destructor or other virtual function.  The module will still be unloaded by
                        // the operating system when the process exits.
                        if( !bIsShutdown )
                        {
                            // Unload the DLL
                            FreeLibrary((HMODULE)ModuleInfo.Handle);
                        }
                        ModuleInfo.Handle = nullptr;
                    }

                    // If we're shutting down, then we never want this module to be "resurrected" in this session.
                    // It's gone for good!  So we'll mark it as such so that we can catch cases where a routine is
                    // trying to load a module that we've unloaded/abandoned at shutdown.
                    if( bIsShutdown )
                    {
                        ModuleInfo.bWasUnloadedAtShutdown = true;
                    }

                    return true;
                }
            }
        }
        return false;
    }
    
    void FModuleManager::AddModule(const FName InModuleName, const FString
                                   InFilename)
    {
        if (Version::IsAtLeast(4, 25))
        {
            ModuleInfoPtr ModuleInfo(new FModuleInfo());
            ModuleInfo.get()->Filename = InFilename;

            Get().AddModuleToModulesList(InModuleName, ModuleInfo);
        }
        else
        {
            ModuleInfoPtrOld ModuleInfo(new FModuleInfoOld());
            ModuleInfo.get()->Filename = InFilename;

            Get().AddModuleToModulesList(InModuleName, ModuleInfo);
        }
    }

    void FModuleManager::AddModuleToModulesList(const FName InModuleName, ModuleInfoPtr& InModuleInfo)
    {
        reinterpret_cast<ModuleManagerNew*>(&Content)->Modules.Add(InModuleName, InModuleInfo);
    }

    void FModuleManager::AddModuleToModulesList(const FName InModuleName,
        ModuleInfoPtrOld &InModuleInfo)
    {
        reinterpret_cast<ModuleManagerOld*>(&Content)->Modules.Add(InModuleName, InModuleInfo);
    }

    FModuleManager::ModuleInfoPtr FModuleManager::FindModule(FName InModuleName)
    {
        ModuleInfoPtr Result = nullptr;

        if (ModuleInfoPtr* FoundModule = reinterpret_cast<ModuleManagerNew*>(&Content)->Modules.Find(InModuleName))
        {
            Result = *FoundModule;
        }

        return Result;
    }

    FModuleManager::ModuleInfoPtrOld FModuleManager::FindModuleOld(
        FName InModuleName) {
      
        ModuleInfoPtrOld Result = nullptr;

        if (ModuleInfoPtrOld* FoundModule = reinterpret_cast<ModuleManagerOld*>(&Content)->Modules.Find(InModuleName))
        {
            Result = *FoundModule;
        }

        return Result;
    }
}
