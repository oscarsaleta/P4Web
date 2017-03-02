#include <vector>
#include <cstdlib>

std::string ScriptHandler::openTempStream(std::string prefix, std::string suffix)
{
    std::string fullname;
    prefix += "XXXXXX" + suffix;
    std::vector<char> dst_prefix(prefix.begin(), prefix.end());
    dst_prefix.push_back('\0');

    int fd = mkstemps(&dst_prefix[0],4);
    if (fd != -1) {
        prefix.assign(dst_prefix.begin(), dst_prefix.end()-1-4);
        fullname = prefix+suffix;
        close(fd);
    }

    globalLogger__.debug("HomeLeft :: created temp file "+fullname);

    return prefix;
}