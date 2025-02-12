#pragma once
#include "behaviortree_cpp/bt_factory.h"
#include "behaviortree_cpp/loggers/groot2_publisher.h"

namespace galaxy {

struct Behavior
{
    BT::Tree                             tree;
    std::shared_ptr<BT::Groot2Publisher> publisher;
};

} // namespace galaxy