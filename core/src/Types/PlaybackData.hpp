#pragma once

namespace IWXMVM::Types
{
    struct AddressAndSize
    {
        uintptr_t address;
        uint32_t size;
    };

    // Contains the addresses of fields needed for
    // rewind gamestate storing & restoring, etc.
    struct PlaybackData
    {
        struct clientActive_t
        {
            uintptr_t snap_serverTime;
            uintptr_t serverTime;
            uintptr_t parseEntitiesNum;
            uintptr_t parseClientsNum;
        } cl;

        struct clientConnection_t
        {
            uintptr_t serverCommandSequence;
            uintptr_t lastExecutedServerCommand;
            AddressAndSize serverCommands;
            // This is sort of a cod4x only field and maps to statPacketSentTime
            // on vanilla cod4; feel free to leave it 0 and core wont write to it
            uintptr_t serverConfigDataSequence;
        } clc;

        struct cgs_t
        {
            uintptr_t serverCommandSequence;
        } cgs;

        struct clientStatic_t
        {
            uintptr_t realtime;
        } cls;

        AddressAndSize s_compassActors;
        AddressAndSize teamChatMsgs;
        AddressAndSize cg_entities;
        AddressAndSize clientInfo;
        AddressAndSize gameState;
        uintptr_t killfeed;
    };
}  // namespace IWXMVM::Types