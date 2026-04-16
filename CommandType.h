#pragma once

enum class CommandType
{
    None = 0,
    Connect = 1,
    Authenticate = 2,
    RequestFileList = 3,
    GetImage = 4,
    Reset = 5,
    StartTransfer = 6,
    TransferChunk = 7,
    TransferComplete = 8,
    Error = 9
};