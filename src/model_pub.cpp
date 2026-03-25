#include <rclcpp/rclcpp.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <fstream>


static const rmw_qos_profile_t qos_profile_latched =
{
  RMW_QOS_POLICY_HISTORY_KEEP_LAST,
  5,
  RMW_QOS_POLICY_RELIABILITY_RELIABLE,
  RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL,
  RMW_QOS_DEADLINE_DEFAULT,
  RMW_QOS_LIFESPAN_DEFAULT,
  RMW_QOS_POLICY_LIVELINESS_SYSTEM_DEFAULT,
  RMW_QOS_LIVELINESS_LEASE_DURATION_DEFAULT,
  false
};

class PointCloudReader : public rclcpp::Node {
public:
    PointCloudReader() : Node("model_reader") {
        // Declare and get the parameter for LAS file path
        this->declare_parameter<std::string>("namespace", "static_publisher");
        this->declare_parameter<std::string>("frame_id", "world");
        this->declare_parameter<std::string>("name", "model");
        this->declare_parameter<std::string>("obj_file_path", "");
        
        this->get_parameter("namespace", ns);
        this->get_parameter("frame_id", frame_id);
        this->get_parameter("name", object_name);
        this->get_parameter("obj_file_path", obj_file_path_);

        // Initialize the publisher
        auto qos = rclcpp::QoS(rclcpp::QoSInitialization(qos_profile_latched.history, 100), qos_profile_latched);
        this->publisher_ = this->create_publisher<visualization_msgs::msg::Marker>(object_name, qos);

        // Create the message
        auto marker = visualization_msgs::msg::Marker();
        marker.header.stamp = this->now();
        marker.header.frame_id = frame_id;

        marker.ns = ns + "_" + object_name;
        marker.id = 0;
        marker.type = visualization_msgs::msg::Marker::MESH_RESOURCE;
        marker.action = visualization_msgs::msg::Marker::ADD;
        marker.mesh_resource = "file://" + obj_file_path_;

        marker.scale.x = 1;
        marker.scale.y = 1;
        marker.scale.z = 1;

        // Publish
        this->publisher_->publish(marker);

        RCLCPP_INFO(this->get_logger(), "Publishing Model %s from %s", object_name.c_str(), obj_file_path_.c_str());
    }

private:

    std::string ns;
    std::string object_name;
    std::string frame_id;
    uint8_t num_publishers = 0;
    std::string obj_file_path_;
    rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr publisher_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PointCloudReader>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
