#pragma once

struct _COMPUTER_INFO
{
    std::string computerName;
    std::string macAddress;
    std::string serialNumber;
    std::string OS;
    std::string manufacturer;
    std::string model;
};

_COMPUTER_INFO getComputerInfor();