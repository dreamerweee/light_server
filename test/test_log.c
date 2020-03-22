#include "../base/log.h"

int main()
{
    LogDebug("begin log test:-----");
    LogInfo("this is info msg.............");
    LogWarn("this is warn msg.............");
    LogError("this is error msg.............");
    LogDebug("end log test:-----");
    return 0;
}