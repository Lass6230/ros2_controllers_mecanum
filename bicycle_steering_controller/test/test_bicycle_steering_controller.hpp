// Copyright (c) 2023, Stogl Robotics Consulting UG (haftungsbeschränkt)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TEST_BICYCLE_STEERING_CONTROLLER_HPP_
#define TEST_BICYCLE_STEERING_CONTROLLER_HPP_

#include <chrono>
#include <limits>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "bicycle_steering_controller/bicycle_steering_controller.hpp"
#include "gmock/gmock.h"
#include "hardware_interface/loaned_command_interface.hpp"
#include "hardware_interface/loaned_state_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "rclcpp/parameter_value.hpp"
#include "rclcpp/time.hpp"
#include "rclcpp/utilities.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"

// TODO(anyone): replace the state and command message types
using ControllerStateMsg =
  steering_controllers_library::SteeringControllersLibrary::AckermanControllerState;
using ControllerReferenceMsg =
  steering_controllers_library::SteeringControllersLibrary::ControllerTwistReferenceMsg;
// using ControllerModeSrvType =
//   steering_controllers_library::SteeringControllersLibrary::ControllerModeSrvType;

namespace
{
constexpr auto NODE_SUCCESS = controller_interface::CallbackReturn::SUCCESS;
constexpr auto NODE_ERROR = controller_interface::CallbackReturn::ERROR;
}  // namespace
// namespace

// subclassing and friending so we can access member variables
class TestableBicycleSteeringController
: public bicycle_steering_controller::BicycleSteeringController
{
  FRIEND_TEST(BicycleSteeringControllerTest, all_parameters_set_configure_success);
  FRIEND_TEST(BicycleSteeringControllerTest, activate_success);
  FRIEND_TEST(BicycleSteeringControllerTest, reactivate_success);
  FRIEND_TEST(BicycleSteeringControllerTest, test_setting_slow_mode_service);
  FRIEND_TEST(BicycleSteeringControllerTest, test_update_logic_fast);
  FRIEND_TEST(BicycleSteeringControllerTest, test_update_logic_slow);
  FRIEND_TEST(BicycleSteeringControllerTest, test_update_logic_chainable_fast);
  FRIEND_TEST(BicycleSteeringControllerTest, test_update_logic_chainable_slow);

public:
  controller_interface::CallbackReturn on_configure(
    const rclcpp_lifecycle::State & previous_state) override
  {
    auto ret = bicycle_steering_controller::BicycleSteeringController::on_configure(previous_state);
    // Only if on_configure is successful create subscription
    if (ret == CallbackReturn::SUCCESS)
    {
      ref_subscriber_wait_set_.add_subscription(ref_subscriber_twist_);
    }
    return ret;
  }

  controller_interface::CallbackReturn on_activate(
    const rclcpp_lifecycle::State & previous_state) override
  {
    auto ref_itfs = on_export_reference_interfaces();
    return bicycle_steering_controller::BicycleSteeringController::on_activate(previous_state);
  }

  /**
   * @brief wait_for_command blocks until a new ControllerReferenceMsg is received.
   * Requires that the executor is not spinned elsewhere between the
   *  message publication and the call to this function.
   *
   * @return true if new ControllerReferenceMsg msg was received, false if timeout.
   */
  bool wait_for_command(
    rclcpp::Executor & executor, rclcpp::WaitSet & subscriber_wait_set,
    const std::chrono::milliseconds & timeout = std::chrono::milliseconds{500})
  {
    bool success = subscriber_wait_set.wait(timeout).kind() == rclcpp::WaitResultKind::Ready;
    if (success)
    {
      executor.spin_some();
    }
    return success;
  }

  bool wait_for_commands(
    rclcpp::Executor & executor,
    const std::chrono::milliseconds & timeout = std::chrono::milliseconds{500})
  {
    return wait_for_command(executor, ref_subscriber_wait_set_, timeout);
  }

  // TODO(anyone): add implementation of any methods of your controller is needed

private:
  rclcpp::WaitSet ref_subscriber_wait_set_;
};

// We are using template class here for easier reuse of Fixture in specializations of controllers
template <typename CtrlType>
class BicycleSteeringControllerFixture : public ::testing::Test
{
public:
  static void SetUpTestCase() {}

  void SetUp()
  {
    // initialize controller
    controller_ = std::make_unique<CtrlType>();

    command_publisher_node_ = std::make_shared<rclcpp::Node>("command_publisher");
    command_publisher_ = command_publisher_node_->create_publisher<ControllerReferenceMsg>(
      "/test_bicycle_steering_controller/commands", rclcpp::SystemDefaultsQoS());

    // service_caller_node_ = std::make_shared<rclcpp::Node>("service_caller");
    // slow_control_service_client_ = service_caller_node_->create_client<ControllerModeSrvType>(
    //   "/test_bicycle_steering_controller/set_slow_control_mode");
  }

  static void TearDownTestCase() {}

  void TearDown() { controller_.reset(nullptr); }

protected:
  void SetUpController(const std::string controller_name = "test_bicycle_steering_controller")
  {
    ASSERT_EQ(controller_->init(controller_name), controller_interface::return_type::OK);

    if (position_feedback_ == true)
    {
      traction_interface_name_ = "position";
    }
    else
    {
      traction_interface_name_ = "velocity";
    }

    std::vector<hardware_interface::LoanedCommandInterface> command_ifs;
    command_itfs_.reserve(joint_command_values_.size());
    command_ifs.reserve(joint_command_values_.size());

    command_itfs_.emplace_back(hardware_interface::CommandInterface(
      rear_wheels_names_[0], traction_interface_name_, &joint_command_values_[0]));
    command_ifs.emplace_back(command_itfs_.back());

    command_itfs_.emplace_back(hardware_interface::CommandInterface(
      front_wheels_names_[0], steering_interface_name_, &joint_command_values_[1]));
    command_ifs.emplace_back(command_itfs_.back());
    // TODO(anyone): Add other command interfaces, if any

    std::vector<hardware_interface::LoanedStateInterface> state_ifs;
    state_itfs_.reserve(joint_state_values_.size());
    state_ifs.reserve(joint_state_values_.size());

    state_itfs_.emplace_back(hardware_interface::StateInterface(
      rear_wheels_names_[0], traction_interface_name_, &joint_state_values_[0]));
    state_ifs.emplace_back(state_itfs_.back());

    state_itfs_.emplace_back(hardware_interface::StateInterface(
      front_wheels_names_[0], steering_interface_name_, &joint_state_values_[1]));
    state_ifs.emplace_back(state_itfs_.back());
    // TODO(anyone): Add other state interfaces, if any

    controller_->assign_interfaces(std::move(command_ifs), std::move(state_ifs));
  }

  // void subscribe_and_get_messages(ControllerStateMsg & msg)
  // {
  //   // create a new subscriber
  //   rclcpp::Node test_subscription_node("test_subscription_node");
  //   auto subs_callback = [&](const ControllerStateMsg::SharedPtr) {};
  //   auto subscription = test_subscription_node.creinterface_name_

  //   // call update to publish the test value
  //   // since update doesn't guarantee a published message, republish until received
  //   int max_sub_check_loop_count = 5;  // max number of tries for pub/sub loop
  //   rclcpp::WaitSet wait_set;          // block used to wait on message
  //   wait_set.add_subscription(subscription);
  //   while (max_sub_check_loop_count--)
  //   {
  //     controller_->update(rclcpp::Time(0), rclcpp::Duration::from_seconds(0.01));
  //     // check if messageparams_ has been received
  //     if (wait_set.wait(std::chrono::milliseconds(2)).kind() == rclcpp::WaitResultKind::Ready)
  //     {
  //       break;
  //     }
  //   }
  //   ASSERT_GE(max_sub_check_loop_count, 0) << "Test was unable to publish a message through "
  //                                             "controller/broadcaster update loop";

  //   // take message from subscription
  //   rclcpp::MessageInfo msg_info;
  //   ASSERT_TRUE(subscription->take(msg, msg_info));
  // }

  // // TODO(anyone): add/remove arguments as it suites your command message type
  // void publish_commands(
  //   const std::vector<double> & displacements = {0.45},
  //   const std::vector<double> & velocities = {0.0}, const double duration = 1.25)
  // {
  //   auto wait_for_topic = [&](const auto topic_name)
  //   {
  //     size_t wait_count = 0;
  //     while (command_publisher_node_->count_subscribers(topic_name) == 0)
  //     {
  //       if (wait_count >= 5)
  //       {
  //         auto error_msg =
  //           std::string("publishing to ") + topic_name + " but no node subscribes to it";
  //         throw std::runtime_error(error_msg);
  //       }
  //       std::this_thread::sleep_for(std::chrono::milliseconds(100));
  //       ++wait_count;
  //     }
  //   };

  //   wait_for_topic(command_publisher_->get_topic_name());

  //   ControllerReferenceMsg msg;
  //   msg.joint_names = joint_names_;
  //   msg.displacements = displacements;
  //   msg.velocities = velocities;
  //   msg.duration = duration;

  //   command_publisher_->publish(msg);
  // }

  // std::shared_ptr<ControllerModeSrvType::Response> call_service(
  //   const bool slow_control, rclcpp::Executor & executor)
  // {
  //   auto request = std::make_shared<ControllerModeSrvType::Request>();
  //   request->data = slow_control;

  //   bool wait_for_service_ret =
  //     slow_control_service_client_->wait_for_service(std::chrono::milliseconds(500));
  //   EXPECT_TRUE(wait_for_service_ret);
  //   if (!wait_for_service_ret)
  //   {
  //     throw std::runtime_error("Services is not available!");
  //   }
  //   auto result = slow_control_service_client_->async_send_request(request);
  //   EXPECT_EQ(executor.spin_until_future_complete(result), rclcpp::FutureReturnCode::SUCCESS);

  //   return result.get();
  // }

protected:
  // TODO(anyone): adjust the members as needed

  // Controller-related parameters

  double reference_timeout_ = 2.0;
  bool front_steering_ = true;
  bool open_loop_ = false;
  unsigned int velocity_rolling_window_size_ = 10;
  bool position_feedback_ = false;
  bool use_stamped_vel_ = true;
  std::vector<std::string> rear_wheels_names_ = {"rear_wheel_joint"};
  std::vector<std::string> front_wheels_names_ = {"steering_axis_joint"};
  double wheelbase_ = 3.24644;
  double front_wheel_radius_ = 0.45;
  double rear_wheels_radius_ = 0.45;

  std::array<double, 2> joint_state_values_ = {1.1, 2.0};
  std::array<double, 2> joint_command_values_ = {2.1, 101.101};
  std::string steering_interface_name_ = "position";

  std::string traction_interface_name_ = "";

  std::vector<hardware_interface::StateInterface> state_itfs_;
  std::vector<hardware_interface::CommandInterface> command_itfs_;

  // Test related parameters
  std::unique_ptr<TestableBicycleSteeringController> controller_;
  rclcpp::Node::SharedPtr command_publisher_node_;
  rclcpp::Publisher<ControllerReferenceMsg>::SharedPtr command_publisher_;
  // rclcpp::Node::SharedPtr service_caller_node_;
  // rclcpp::Client<ControllerModeSrvType>::SharedPtr slow_control_service_client_;
};

#endif  // TEST_BICYCLE_STEERING_CONTROLLER_HPP_
