#include "behaviortree_cpp/bt_factory.h"

class FindNextSystem : public BT::SyncActionNode
{
public:
    FindNextSystem(const std::string& name)
      : BT::SyncActionNode(name, {})
    {
    }

    // You must override the virtual function tick()
    BT::NodeStatus tick() override
    {
        std::cout << "ApproachObject: " << this->name() << std::endl;
        return BT::NodeStatus::SUCCESS;
    }
};

int main(int argc, char** argv)
try
{
    BT::BehaviorTreeFactory factory;

    // The recommended way to create a Node is through inheritance.
    factory.registerNodeType<FindNextSystem>("FindNextSystem");

    // Registering a SimpleActionNode using a function pointer.
    // You can use C++11 lambdas or std::bind
    // factory.registerSimpleCondition("CheckBattery", [&](TreeNode&) { return CheckBattery(); });

    // You can also create SimpleActionNodes using methods of a class
    // GripperInterface gripper;
    // factory.registerSimpleAction("CloseGripper", [&](TreeNode&) { return gripper.close(); });

    // Trees are created at deployment-time (i.e. at run-time, but only
    // once at the beginning).

    // IMPORTANT: when the object "tree" goes out of scope, all the
    // TreeNodes are destroyed
    auto tree = factory.createTreeFromFile("../data/behaviors/drones.xml");

    // To "execute" a Tree you need to "tick" it.
    // The tick is propagated to the children based on the logic of the tree.
    // In this case, the entire sequence is executed, because all the children
    // of the Sequence return SUCCESS.
    tree.tickWhileRunning();

    return 0;
}
catch (const std::exception& e)
{
    std::cerr << "Terminated: " << e.what() << std::endl;
    return -1;
}