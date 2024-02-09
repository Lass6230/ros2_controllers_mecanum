// auto-generated DO NOT EDIT

#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <limits>
#include <mutex>
#include <rclcpp/node.hpp>
#include <rclcpp_lifecycle/lifecycle_node.hpp>
#include <rclcpp/logger.hpp>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <parameter_traits/parameter_traits.hpp>

#include <rsl/static_string.hpp>
#include <rsl/static_vector.hpp>
#include <rsl/parameter_validators.hpp>



namespace mecanum_drive_controller {

// Use validators from RSL
using rsl::unique;
using rsl::subset_of;
using rsl::fixed_size;
using rsl::size_gt;
using rsl::size_lt;
using rsl::not_empty;
using rsl::element_bounds;
using rsl::lower_element_bounds;
using rsl::upper_element_bounds;
using rsl::bounds;
using rsl::lt;
using rsl::gt;
using rsl::lt_eq;
using rsl::gt_eq;
using rsl::one_of;
using rsl::to_parameter_result_msg;

// temporarily needed for backwards compatibility for custom validators
using namespace parameter_traits;

template <typename T>
[[nodiscard]] auto to_parameter_value(T value) {
    return rclcpp::ParameterValue(value);
}

template <size_t capacity>
[[nodiscard]] auto to_parameter_value(rsl::StaticString<capacity> const& value) {
    return rclcpp::ParameterValue(rsl::to_string(value));
}

template <typename T, size_t capacity>
[[nodiscard]] auto to_parameter_value(rsl::StaticVector<T, capacity> const& value) {
    return rclcpp::ParameterValue(rsl::to_vector(value));
}
    struct Params {
        double reference_timeout = 0.0;
        std::vector<std::string> command_joint_names = {};
        std::vector<std::string> state_joint_names = {};
        std::string interface_name = "";
        std::string base_frame_id = "base_link";
        std::string odom_frame_id = "odom";
        bool enable_odom_tf = true;
        std::vector<double> twist_covariance_diagonal = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
        std::vector<double> pose_covariance_diagonal = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
        struct Kinematics {
            double wheels_radius = 0.0;
            double sum_of_robot_center_projection_on_X_Y_axis = 0.0;
            struct BaseFrameOffset {
                double x = 0.0;
                double y = 0.0;
                double theta = 0.0;
            } base_frame_offset;
        } kinematics;
        // for detecting if the parameter struct has been updated
        rclcpp::Time __stamp;
    };
    struct StackParams {
        double reference_timeout = 0.0;
        bool enable_odom_tf = true;
        struct Kinematics {
            double wheels_radius = 0.0;
            double sum_of_robot_center_projection_on_X_Y_axis = 0.0;
            struct BaseFrameOffset {
                double x = 0.0;
                double y = 0.0;
                double theta = 0.0;
            } base_frame_offset;
        } kinematics;
    };

  class ParamListener{
  public:
    // throws rclcpp::exceptions::InvalidParameterValueException on initialization if invalid parameter are loaded
    ParamListener(rclcpp::Node::SharedPtr node, std::string const& prefix = "")
    : ParamListener(node->get_node_parameters_interface(), node->get_logger(), prefix) {}

    ParamListener(rclcpp_lifecycle::LifecycleNode::SharedPtr node, std::string const& prefix = "")
    : ParamListener(node->get_node_parameters_interface(), node->get_logger(), prefix) {}

    ParamListener(const std::shared_ptr<rclcpp::node_interfaces::NodeParametersInterface>& parameters_interface,
                  std::string const& prefix = "")
    : ParamListener(parameters_interface, rclcpp::get_logger("mecanum_drive_controller"), prefix) {
      RCLCPP_DEBUG(logger_, "ParameterListener: Not using node logger, recommend using other constructors to use a node logger");
    }

    ParamListener(const std::shared_ptr<rclcpp::node_interfaces::NodeParametersInterface>& parameters_interface,
                  rclcpp::Logger logger, std::string const& prefix = "") {
      logger_ = logger;
      prefix_ = prefix;
      if (!prefix_.empty() && prefix_.back() != '.') {
        prefix_ += ".";
      }

      parameters_interface_ = parameters_interface;
      declare_params();
      auto update_param_cb = [this](const std::vector<rclcpp::Parameter> &parameters){return this->update(parameters);};
      handle_ = parameters_interface_->add_on_set_parameters_callback(update_param_cb);
      clock_ = rclcpp::Clock();
    }

    Params get_params() const{
      std::lock_guard<std::mutex> lock(mutex_);
      return params_;
    }

    bool is_old(Params const& other) const {
      std::lock_guard<std::mutex> lock(mutex_);
      return params_.__stamp != other.__stamp;
    }

    StackParams get_stack_params() {
      Params params = get_params();
      StackParams output;
      output.reference_timeout = params.reference_timeout;
      output.kinematics.base_frame_offset.x = params.kinematics.base_frame_offset.x;
      output.kinematics.base_frame_offset.y = params.kinematics.base_frame_offset.y;
      output.kinematics.base_frame_offset.theta = params.kinematics.base_frame_offset.theta;
      output.kinematics.wheels_radius = params.kinematics.wheels_radius;
      output.kinematics.sum_of_robot_center_projection_on_X_Y_axis = params.kinematics.sum_of_robot_center_projection_on_X_Y_axis;
      output.enable_odom_tf = params.enable_odom_tf;

      return output;
    }

    void refresh_dynamic_parameters() {
      auto updated_params = get_params();
      // TODO remove any destroyed dynamic parameters

      // declare any new dynamic parameters
      rclcpp::Parameter param;

    }

    rcl_interfaces::msg::SetParametersResult update(const std::vector<rclcpp::Parameter> &parameters) {
      auto updated_params = get_params();

      for (const auto &param: parameters) {
        if (param.get_name() == (prefix_ + "reference_timeout")) {
            updated_params.reference_timeout = param.as_double();
            RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
        }
        if (param.get_name() == (prefix_ + "command_joint_names")) {
            updated_params.command_joint_names = param.as_string_array();
            RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
        }
        if (param.get_name() == (prefix_ + "state_joint_names")) {
            updated_params.state_joint_names = param.as_string_array();
            RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
        }
        if (param.get_name() == (prefix_ + "interface_name")) {
            updated_params.interface_name = param.as_string();
            RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
        }
        if (param.get_name() == (prefix_ + "kinematics.base_frame_offset.x")) {
            updated_params.kinematics.base_frame_offset.x = param.as_double();
            RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
        }
        if (param.get_name() == (prefix_ + "kinematics.base_frame_offset.y")) {
            updated_params.kinematics.base_frame_offset.y = param.as_double();
            RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
        }
        if (param.get_name() == (prefix_ + "kinematics.base_frame_offset.theta")) {
            updated_params.kinematics.base_frame_offset.theta = param.as_double();
            RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
        }
        if (param.get_name() == (prefix_ + "kinematics.wheels_radius")) {
            updated_params.kinematics.wheels_radius = param.as_double();
            RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
        }
        if (param.get_name() == (prefix_ + "kinematics.sum_of_robot_center_projection_on_X_Y_axis")) {
            updated_params.kinematics.sum_of_robot_center_projection_on_X_Y_axis = param.as_double();
            RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
        }
        if (param.get_name() == (prefix_ + "base_frame_id")) {
            updated_params.base_frame_id = param.as_string();
            RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
        }
        if (param.get_name() == (prefix_ + "odom_frame_id")) {
            updated_params.odom_frame_id = param.as_string();
            RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
        }
        if (param.get_name() == (prefix_ + "enable_odom_tf")) {
            updated_params.enable_odom_tf = param.as_bool();
            RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
        }
        if (param.get_name() == (prefix_ + "twist_covariance_diagonal")) {
            updated_params.twist_covariance_diagonal = param.as_double_array();
            RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
        }
        if (param.get_name() == (prefix_ + "pose_covariance_diagonal")) {
            updated_params.pose_covariance_diagonal = param.as_double_array();
            RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
        }
      }

      updated_params.__stamp = clock_.now();
      update_interal_params(updated_params);
      return rsl::to_parameter_result_msg({});
    }

    void declare_params(){
      auto updated_params = get_params();
      // declare all parameters and give default values to non-required ones
      if (!parameters_interface_->has_parameter(prefix_ + "reference_timeout")) {
          rcl_interfaces::msg::ParameterDescriptor descriptor;
          descriptor.description = "Timeout for controller references after which they will be reset. This is especially useful for controllers that can cause unwanted and dangerous behavior if reference is not reset, e.g., velocity controllers. If value is 0 the reference is reset after each run.";
          descriptor.read_only = false;
          auto parameter = to_parameter_value(updated_params.reference_timeout);
          parameters_interface_->declare_parameter(prefix_ + "reference_timeout", parameter, descriptor);
      }
      if (!parameters_interface_->has_parameter(prefix_ + "command_joint_names")) {
          rcl_interfaces::msg::ParameterDescriptor descriptor;
          descriptor.description = "Name of the wheels joints.";
          descriptor.read_only = false;
          auto parameter = to_parameter_value(updated_params.command_joint_names);
          parameters_interface_->declare_parameter(prefix_ + "command_joint_names", parameter, descriptor);
      }
      if (!parameters_interface_->has_parameter(prefix_ + "state_joint_names")) {
          rcl_interfaces::msg::ParameterDescriptor descriptor;
          descriptor.description = "(optional) Specifies joints for reading states. This parameter is only relevant when state joints are different then command joint, i.e., when a following controller is used.";
          descriptor.read_only = true;
          auto parameter = to_parameter_value(updated_params.state_joint_names);
          parameters_interface_->declare_parameter(prefix_ + "state_joint_names", parameter, descriptor);
      }
      if (!parameters_interface_->has_parameter(prefix_ + "interface_name")) {
          rcl_interfaces::msg::ParameterDescriptor descriptor;
          descriptor.description = "Name of the interface used by the controller for sending commands, reading states and getting references.";
          descriptor.read_only = true;
          auto parameter = to_parameter_value(updated_params.interface_name);
          parameters_interface_->declare_parameter(prefix_ + "interface_name", parameter, descriptor);
      }
      if (!parameters_interface_->has_parameter(prefix_ + "kinematics.base_frame_offset.x")) {
          rcl_interfaces::msg::ParameterDescriptor descriptor;
          descriptor.description = "Base frame offset along X axis of base_frame (base_link frame).";
          descriptor.read_only = true;
          auto parameter = to_parameter_value(updated_params.kinematics.base_frame_offset.x);
          parameters_interface_->declare_parameter(prefix_ + "kinematics.base_frame_offset.x", parameter, descriptor);
      }
      if (!parameters_interface_->has_parameter(prefix_ + "kinematics.base_frame_offset.y")) {
          rcl_interfaces::msg::ParameterDescriptor descriptor;
          descriptor.description = "Base frame offset along Y axis of base_frame (base_link frame).";
          descriptor.read_only = true;
          auto parameter = to_parameter_value(updated_params.kinematics.base_frame_offset.y);
          parameters_interface_->declare_parameter(prefix_ + "kinematics.base_frame_offset.y", parameter, descriptor);
      }
      if (!parameters_interface_->has_parameter(prefix_ + "kinematics.base_frame_offset.theta")) {
          rcl_interfaces::msg::ParameterDescriptor descriptor;
          descriptor.description = "Base frame offset along Theta axis of base_frame (base_link frame).";
          descriptor.read_only = true;
          auto parameter = to_parameter_value(updated_params.kinematics.base_frame_offset.theta);
          parameters_interface_->declare_parameter(prefix_ + "kinematics.base_frame_offset.theta", parameter, descriptor);
      }
      if (!parameters_interface_->has_parameter(prefix_ + "kinematics.wheels_radius")) {
          rcl_interfaces::msg::ParameterDescriptor descriptor;
          descriptor.description = "Wheel's radius.";
          descriptor.read_only = false;
          auto parameter = to_parameter_value(updated_params.kinematics.wheels_radius);
          parameters_interface_->declare_parameter(prefix_ + "kinematics.wheels_radius", parameter, descriptor);
      }
      if (!parameters_interface_->has_parameter(prefix_ + "kinematics.sum_of_robot_center_projection_on_X_Y_axis")) {
          rcl_interfaces::msg::ParameterDescriptor descriptor;
          descriptor.description = "Wheels geometric param used in mecanum wheels' IK. lx and ly represent the distance from the robot's center to the wheels projected on the x and y axis with origin at robots center respectively, sum_of_robot_center_projection_on_X_Y_axis = lx+ly";
          descriptor.read_only = false;
          auto parameter = to_parameter_value(updated_params.kinematics.sum_of_robot_center_projection_on_X_Y_axis);
          parameters_interface_->declare_parameter(prefix_ + "kinematics.sum_of_robot_center_projection_on_X_Y_axis", parameter, descriptor);
      }
      if (!parameters_interface_->has_parameter(prefix_ + "base_frame_id")) {
          rcl_interfaces::msg::ParameterDescriptor descriptor;
          descriptor.description = "Base frame_id set to value of base_frame_id.";
          descriptor.read_only = false;
          auto parameter = to_parameter_value(updated_params.base_frame_id);
          parameters_interface_->declare_parameter(prefix_ + "base_frame_id", parameter, descriptor);
      }
      if (!parameters_interface_->has_parameter(prefix_ + "odom_frame_id")) {
          rcl_interfaces::msg::ParameterDescriptor descriptor;
          descriptor.description = "Odometry frame_id set to value of odom_frame_id.";
          descriptor.read_only = false;
          auto parameter = to_parameter_value(updated_params.odom_frame_id);
          parameters_interface_->declare_parameter(prefix_ + "odom_frame_id", parameter, descriptor);
      }
      if (!parameters_interface_->has_parameter(prefix_ + "enable_odom_tf")) {
          rcl_interfaces::msg::ParameterDescriptor descriptor;
          descriptor.description = "Publishing to tf is enabled or disabled?";
          descriptor.read_only = false;
          auto parameter = to_parameter_value(updated_params.enable_odom_tf);
          parameters_interface_->declare_parameter(prefix_ + "enable_odom_tf", parameter, descriptor);
      }
      if (!parameters_interface_->has_parameter(prefix_ + "twist_covariance_diagonal")) {
          rcl_interfaces::msg::ParameterDescriptor descriptor;
          descriptor.description = "Diagonal values of twist covariance matrix.";
          descriptor.read_only = false;
          auto parameter = to_parameter_value(updated_params.twist_covariance_diagonal);
          parameters_interface_->declare_parameter(prefix_ + "twist_covariance_diagonal", parameter, descriptor);
      }
      if (!parameters_interface_->has_parameter(prefix_ + "pose_covariance_diagonal")) {
          rcl_interfaces::msg::ParameterDescriptor descriptor;
          descriptor.description = "Diagonal values of pose covariance matrix.";
          descriptor.read_only = false;
          auto parameter = to_parameter_value(updated_params.pose_covariance_diagonal);
          parameters_interface_->declare_parameter(prefix_ + "pose_covariance_diagonal", parameter, descriptor);
      }
      // get parameters and fill struct fields
      rclcpp::Parameter param;
      param = parameters_interface_->get_parameter(prefix_ + "reference_timeout");
      RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
      updated_params.reference_timeout = param.as_double();
      param = parameters_interface_->get_parameter(prefix_ + "command_joint_names");
      RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
      updated_params.command_joint_names = param.as_string_array();
      param = parameters_interface_->get_parameter(prefix_ + "state_joint_names");
      RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
      updated_params.state_joint_names = param.as_string_array();
      param = parameters_interface_->get_parameter(prefix_ + "interface_name");
      RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
      updated_params.interface_name = param.as_string();
      param = parameters_interface_->get_parameter(prefix_ + "kinematics.base_frame_offset.x");
      RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
      updated_params.kinematics.base_frame_offset.x = param.as_double();
      param = parameters_interface_->get_parameter(prefix_ + "kinematics.base_frame_offset.y");
      RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
      updated_params.kinematics.base_frame_offset.y = param.as_double();
      param = parameters_interface_->get_parameter(prefix_ + "kinematics.base_frame_offset.theta");
      RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
      updated_params.kinematics.base_frame_offset.theta = param.as_double();
      param = parameters_interface_->get_parameter(prefix_ + "kinematics.wheels_radius");
      RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
      updated_params.kinematics.wheels_radius = param.as_double();
      param = parameters_interface_->get_parameter(prefix_ + "kinematics.sum_of_robot_center_projection_on_X_Y_axis");
      RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
      updated_params.kinematics.sum_of_robot_center_projection_on_X_Y_axis = param.as_double();
      param = parameters_interface_->get_parameter(prefix_ + "base_frame_id");
      RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
      updated_params.base_frame_id = param.as_string();
      param = parameters_interface_->get_parameter(prefix_ + "odom_frame_id");
      RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
      updated_params.odom_frame_id = param.as_string();
      param = parameters_interface_->get_parameter(prefix_ + "enable_odom_tf");
      RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
      updated_params.enable_odom_tf = param.as_bool();
      param = parameters_interface_->get_parameter(prefix_ + "twist_covariance_diagonal");
      RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
      updated_params.twist_covariance_diagonal = param.as_double_array();
      param = parameters_interface_->get_parameter(prefix_ + "pose_covariance_diagonal");
      RCLCPP_DEBUG_STREAM(logger_, param.get_name() << ": " << param.get_type_name() << " = " << param.value_to_string());
      updated_params.pose_covariance_diagonal = param.as_double_array();


      updated_params.__stamp = clock_.now();
      update_interal_params(updated_params);
    }

    private:
      void update_interal_params(Params updated_params) {
        std::lock_guard<std::mutex> lock(mutex_);
        params_ = updated_params;
      }

      std::string prefix_;
      Params params_;
      rclcpp::Clock clock_;
      std::shared_ptr<rclcpp::node_interfaces::OnSetParametersCallbackHandle> handle_;
      std::shared_ptr<rclcpp::node_interfaces::NodeParametersInterface> parameters_interface_;

      // rclcpp::Logger cannot be default-constructed
      // so we must provide a initialization here even though
      // every one of our constructors initializes logger_
      rclcpp::Logger logger_ = rclcpp::get_logger("mecanum_drive_controller");
      std::mutex mutable mutex_;
  };

} // namespace mecanum_drive_controller
