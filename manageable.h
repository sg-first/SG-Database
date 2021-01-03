#pragma once

class manageable
{
private:
    bool userManage=true;

public:
    bool getOwnership() { return this->userManage; }
    void setSystemManage() { this->userManage=true; }
    virtual ~manageable() {}
};
