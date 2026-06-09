#include "StdInclude.hpp"
#include "Functions.hpp"

#include "Addresses.hpp"

namespace IWXMVM::T4::Functions
{
    Structures::dvar_s* FindDvar(const std::string_view name)
    {
        typedef Structures::dvar_s*(__cdecl * Dvar_FindVar_t)(const char* name);
        Dvar_FindVar_t Dvar_FindVar_Internal = (Dvar_FindVar_t)GetGameAddresses().Dvar_FindMalleableVar();

        return Dvar_FindVar_Internal(name.data());
    }

    std::string GetFilePath(const std::string_view demoName)
    {
        auto searchpath = (Structures::searchpath_s*)GetGameAddresses().fs_searchpaths();
        while (searchpath->next)
        {
            searchpath = searchpath->next;
            if (!searchpath->dir)
                continue;

            auto path = std::filesystem::path(searchpath->dir->path);
            path.append(searchpath->dir->gamedir);
            path.append("demos");
            path.append(demoName);

            if (std::filesystem::exists(path))
                return path.string();
        }

        return "";
    }

    void Cbuf_AddText(std::string command)
    {
        LOG_DEBUG("Executing command \"{0}\"", command);

        command.append("\n");

        const char* commandString = command.c_str();
        const auto Cbuf_AddText_Address = GetGameAddresses().Cbuf_AddText();

        __asm
        {
            mov eax, commandString
            mov ecx, 0
            call Cbuf_AddText_Address
        }
    }

    uint16_t SL_GetStringOfSize(const char* string, int entityType, int stringLength)
    {
        typedef uint16_t(__cdecl * SL_GetStringOfSize_t)(const char*, int, int);
        SL_GetStringOfSize_t SL_GetStringOfSize = (SL_GetStringOfSize_t)GetGameAddresses().SL_GetStringOfSize();

        return SL_GetStringOfSize(string, entityType, stringLength);
    }

    
    bool CG_DObjGetWorldBoneMatrix(Structures::centity_s* entity /*@<eax>*/, int boneIndex /*@<ecx>*/, 
                                   float* matrix /*@<esi>*/, Structures::DObj_s* dobj, float* origin)
    {
        // TODO: 
        return false;
        /*static uintptr_t address = GetGameAddresses().CG_DObjGetWorldBoneMatrix();
        __asm 
        {
            mov eax, entity
            mov ecx, boneIndex
            mov esi, matrix
            push origin
            push dobj
            call address
            add esp, 8
        }*/
    }


    Structures::Material* Material_RegisterHandle(const char* materialName)
    {
        typedef Structures::Material*(__cdecl * Material_RegisterHandle_t)(const char* materialName, int a2);
        Material_RegisterHandle_t Material_RegisterHandle =
            reinterpret_cast<Material_RegisterHandle_t>(GetGameAddresses().Material_RegisterHandle());

        return Material_RegisterHandle(materialName, 3);
    }

    void Dvar_SetStringByName(const char* dvarName, const char* value)
    {
        static uintptr_t address = GetGameAddresses().Dvar_SetStringByName();
        __asm 
        {
            mov eax, dvarName
            push value
            call address
            add esp, 4
        }
    }

}  // namespace IWXMVM::T4::Functions
