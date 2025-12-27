#pragma once
#include "Attachment.h"
#include <string>
#include <vector>

struct Email
{
    std::string from;
    std::string to;
    std::string subject;
    std::string body;
    std::vector<Attachment> attachments;
};
