#include <functional>
#include <string>
#include <iostream>
#include <cmath>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/common/common.hh>
#include <ros/ros.h>
#include <ros/console.h>
#include <tf2_ros/transform_broadcaster.h>
#include <tf/transform_datatypes.h>
#include <omnivelma_msgs/Vels.h>
#include <omnivelma_msgs/SetFriction.h>
#include <omnivelma_msgs/EncodersStamped.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/Quaternion.h>
#include <std_msgs/Float64.h>
#include <nav_msgs/Odometry.h>
#include <omnivelma_msgs/SetInertia.h>
#include <boost/lexical_cast.hpp>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include <ignition/math.hh>

#define MODEL_NAME std::string("omnivelma")
///Długość jest równa sqrt(2)/2 aby tworzyć kąt 45°
#define AXIS_LENGTH 0.707106781186548
#define CLIENT_NAME "gazebo_ros"
#define MAP_TF "odom"
#define MAX_FORCE 150.0

namespace gazebo
{
///Klasa sterująca platformą
class Omnivelma : public ModelPlugin
{
public:
	Omnivelma()
	{
		counter = 0;
		velRR = 0;
		velFR = 0;
		velRL = 0;
		velFL = 0;
	}

public:
	///Uruchamiane na inicjalizację
	void Load(physics::ModelPtr parent, sdf::ElementPtr sdf)
	{
		model = parent;
			
		updateConnection = event::Events::ConnectWorldUpdateBegin(std::bind(&Omnivelma::OnUpdate, this));
		
		linkPrefix = std::string(model -> GetName()).append("::").append(MODEL_NAME).append("::");

		std::string topicPrefix = std::string("/").append(model -> GetName()).append("/");

		//odszukaj obiekty kół
		physics::LinkPtr wheelRR = model ->  GetLink(linkPrefix + "wheel_rr");
		physics::LinkPtr wheelRL = model ->  GetLink(linkPrefix + "wheel_rl");
		physics::LinkPtr wheelFR = model ->  GetLink(linkPrefix + "wheel_fr");
		physics::LinkPtr wheelFL = model ->  GetLink(linkPrefix + "wheel_fl");
		
		if(!wheelRR || !wheelRL || !wheelFR || !wheelFL)
		{
			ROS_FATAL_STREAM("Nie udało się znaleźć obiektów kół");
		}

		//odszukaj kolizje kół
		wheelRRCollision = wheelRR -> GetCollision("wheel_rr_collision");
		wheelRLCollision = wheelRL -> GetCollision("wheel_rl_collision");
		wheelFRCollision = wheelFR -> GetCollision("wheel_fr_collision");
		wheelFLCollision = wheelFL -> GetCollision("wheel_fl_collision");
		
		if(!wheelRRCollision || !wheelRLCollision || !wheelFRCollision || !wheelFLCollision)
		{
			ROS_FATAL_STREAM("Nie udało się znaleźć kolizji kół modelu");
		}

		motorRR = model -> GetJoint(linkPrefix + "motor_rr");
		motorRL = model -> GetJoint(linkPrefix + "motor_rl");
		motorFR = model -> GetJoint(linkPrefix + "motor_fr");
		motorFL = model -> GetJoint(linkPrefix + "motor_fl");

		motorRR -> SetParam("fmax", 0, MAX_FORCE);
		motorRL -> SetParam("fmax", 0, MAX_FORCE);
		motorFR -> SetParam("fmax", 0, MAX_FORCE);
		motorFL -> SetParam("fmax", 0, MAX_FORCE);

		rkP = model -> WorldPose().Rot();
		rkQ = model -> WorldPose().Rot();
		rkPN = model ->WorldPose().Rot();
		rkQN = model -> WorldPose().Rot();

		if(!motorRR || !motorRL || !motorFR || !motorFL)
		{
			ROS_FATAL_STREAM("Nie udało się znaleźć przegubów silników");
		}

		//inicjalizacja ROSa
		if (!ros::isInitialized())
		{
			int argc = 0;
			char **argv = nullptr;
			ros::init(argc, argv, CLIENT_NAME, ros::init_options::NoSigintHandler);
		}

		//stwórz Node dla ROSa
		rosNode.reset(new ros::NodeHandle());

		//stwórz topic do odbierania prędkości kół
		rosSub = rosNode -> subscribe<omnivelma_msgs::Vels>(topicPrefix + "vels", 10, std::bind(&Omnivelma::OnRosMsg, this, std::placeholders::_1));
		if(!rosSub)
		{
			ROS_FATAL_STREAM("Nie udało się stworzyć odbiornika " << topicPrefix << "vels");
		}

		rosKinectSub = rosNode -> subscribe<std_msgs::Float64>(topicPrefix + "kinect_rotation", 10, std::bind(&Omnivelma::OnRosKinectMsg, this, std::placeholders::_1));
		if(!rosKinectSub)
		{
			ROS_FATAL_STREAM("Nie udało się stworzyć odbiornika " << topicPrefix << "kinect_rotation");
		}

		//stwórz topic do nadawania pozycji
		rosPose = rosNode -> advertise<geometry_msgs::PoseStamped>(topicPrefix + "pose", 10);
		if(!rosPose)
		{
			ROS_FATAL_STREAM("Nie udało się stworzyć nadajnika " << topicPrefix << "pose");
		}

		//stwórz topic do nadawania enkoderów
		rosEnc = rosNode -> advertise<omnivelma_msgs::EncodersStamped>(topicPrefix + "encoders", 10);
		if(!rosEnc)
		{
			ROS_FATAL_STREAM("Nie udało się stworzyć nadajnika " << topicPrefix << "encoders");
		}

		//stwórz topic do nadawania prędkości
		rosTwist = rosNode -> advertise<geometry_msgs::TwistStamped>(topicPrefix + "twist", 10);
		if(!rosTwist)
		{
			ROS_FATAL_STREAM("Nie udało się stworzyć nadajnika " << topicPrefix << "twist");
		}

		//stwórz serwer do odbierania tarcia
		ros::AdvertiseServiceOptions aso = ros::AdvertiseServiceOptions::create<omnivelma_msgs::SetFriction>(topicPrefix + "set_friction", std::bind(&Omnivelma::SetFriction, this, std::placeholders::_1, std::placeholders::_2), nullptr, nullptr);
		rosFri = rosNode -> advertiseService(aso);
		if(!rosFri)
		{
			ROS_FATAL_STREAM("Nie udało się stworzyć serwera " << topicPrefix << "set_friction");
		}

		//stwórz serwer do odbierania inercji
		ros::AdvertiseServiceOptions asi = ros::AdvertiseServiceOptions::create<omnivelma_msgs::SetInertia>(topicPrefix + "set_inertia", std::bind(&Omnivelma::SetInertia, this, std::placeholders::_1, std::placeholders::_2), nullptr, nullptr);
		rosIne = rosNode -> advertiseService(asi);
		if(!rosIne)
		{
			ROS_FATAL_STREAM("Nie udało się stworzyć serwera " << topicPrefix << "set_inertia");
		}
		
		//Autor: Piotr Walas
		// rosOdom = rosNode->advertise<nav_msgs::Odometry>("odom", 1);
		// if(!rosOdom)
		// {
		// 	ROS_FATAL_STREAM("Nie udało się stworzyć nadajnika " << "odom");
		// }
		
		//powiadom o gotowości
		ROS_DEBUG_STREAM("Omnivelma zainicjalizowana");
	}


private:

	void SetVelocities()
	{
		//nadaj poprzednie prędkości kół
		if(!std::isnan(velRR))
			motorRR -> SetParam("vel", 0, velRR);
		if(!std::isnan(velRL))
			motorRL -> SetParam("vel", 0, velRL);
		if(!std::isnan(velFR))
			motorFR -> SetParam("vel", 0, velFR);
		if(!std::isnan(velFL))
			motorFL -> SetParam("vel", 0, velFL);
	}

	void SetKinectRotation(double angle)
	{
		ignition::math::Quaternion<double> q;
		ignition::math::Quaternion<double> mq =  model -> WorldPose().Rot();
		q.Euler(0.0,0.0,angle - mq.Yaw());
		rkQ = q;
		rkQ.Normalize();
	}

	///Funkcja podłączana do zdarzenia aktualizacji
	void OnUpdate()
	{
		//ustaw kierunek wektorów tarcia
		//kierunek wektora o większym współczynniku tarcia jest ustalony w lokalnym, dla koła, układzie współrzędnych
		//zatem wektor należy obrócić zgodnie z modelem i odwrotnie do obrotu koła, aby wyjściowo był w płaszczyźnie platformy
		const ignition::math::Quaternion<double>  modelRot = model -> WorldPose().Rot();
		const ignition::math::Quaternion<double>  wheelRRRot = wheelRRCollision -> WorldPose().Rot();
		const ignition::math::Quaternion<double>  wheelRLRot = wheelRLCollision -> WorldPose().Rot();
		const ignition::math::Quaternion<double>  wheelFRRot = wheelFRCollision -> WorldPose().Rot();
		const ignition::math::Quaternion<double>  wheelFLRot = wheelFLCollision -> WorldPose().Rot();

		wheelRRCollision -> GetSurface() -> FrictionPyramid() -> direction1 = wheelRRRot.RotateVectorReverse(modelRot.RotateVector(ignition::math::Vector3<double>(AXIS_LENGTH, -AXIS_LENGTH, 0)));
		wheelRLCollision -> GetSurface() -> FrictionPyramid() -> direction1 = wheelRLRot.RotateVectorReverse(modelRot.RotateVector(ignition::math::Vector3<double>(AXIS_LENGTH, AXIS_LENGTH, 0)));
		wheelFRCollision -> GetSurface() -> FrictionPyramid() -> direction1 = wheelFRRot.RotateVectorReverse(modelRot.RotateVector(ignition::math::Vector3<double>(-AXIS_LENGTH, -AXIS_LENGTH, 0)));
		wheelFLCollision -> GetSurface() -> FrictionPyramid() -> direction1 = wheelFLRot.RotateVectorReverse(modelRot.RotateVector(ignition::math::Vector3<double>(-AXIS_LENGTH, AXIS_LENGTH, 0)));

		//ROS_WARN_STREAM("VELMA: " + boost::lexical_cast<std::string>(kinectRot.x) + " " + boost::lexical_cast<std::string>(kinectRot.y) + " " + boost::lexical_cast<std::string>(kinectRot.z) + " " + boost::lexical_cast<std::string>(kinectRot.w));

		//wyślij pozycję
		const ignition::math::Pose3d& pose = model -> WorldPose();
		geometry_msgs::PoseStamped poseMsg;
		poseMsg.pose.position.x = pose.Pos().X();
		poseMsg.pose.position.y = pose.Pos().Y();
		poseMsg.pose.position.z = pose.Pos().Z();
		poseMsg.pose.orientation.x = pose.Rot().X();
		poseMsg.pose.orientation.y = pose.Rot().Y();
		poseMsg.pose.orientation.z = pose.Rot().Z();
		poseMsg.pose.orientation.w = pose.Rot().W();
		poseMsg.header.seq = counter;
		poseMsg.header.stamp = ros::Time::now();
		poseMsg.header.frame_id = "map"; // - real pose
		rosPose.publish(poseMsg);

		//wyślij enkodery
		omnivelma_msgs::EncodersStamped encMsg;
		encMsg.encoders.velocity.rr = motorRR -> GetVelocity(0);
		encMsg.encoders.angle.rr = motorRR -> Position(0);
		encMsg.encoders.velocity.rl = motorRL -> GetVelocity(0);
		encMsg.encoders.angle.rl = motorRL -> Position(0);
		encMsg.encoders.velocity.fr = motorFR -> GetVelocity(0);
		encMsg.encoders.angle.fr = motorFR -> Position(0);
		encMsg.encoders.velocity.fl = motorFL -> GetVelocity(0);
		encMsg.encoders.angle.fl = motorFL -> Position(0);
		encMsg.header.seq = counter;
		encMsg.header.stamp = ros::Time::now();
		encMsg.header.frame_id = "0";
		rosEnc.publish(encMsg);

		//wyślij prędkość
		const ignition::math::Vector3d linVel = model-> RelativeLinearVel();
		const ignition::math::Vector3d angVel = model -> RelativeAngularVel();
		geometry_msgs::TwistStamped twistMsg;
		twistMsg.twist.linear.x = linVel.X();
		twistMsg.twist.linear.y = linVel.Y();
		twistMsg.twist.linear.z = linVel.Z();
		twistMsg.twist.angular.x = angVel.X();
		twistMsg.twist.angular.y = angVel.Y();
		twistMsg.twist.angular.z = angVel.Z();
		twistMsg.header.seq = counter;
		twistMsg.header.stamp = ros::Time::now();
		twistMsg.header.frame_id = MAP_TF;
		rosTwist.publish(twistMsg);
		
		//Autor: Piotr Walas
		//wyslij odometrie
		// nav_msgs::Odometry odometryMsg;
		// odometryMsg.header.seq = counter;
		// odometryMsg.header.stamp = ros::Time::now();
		// // new TODO
		// odometryMsg.child_frame_id = "base_footprint";
		// odometryMsg.header.frame_id = MAP_TF;
		// odometryMsg.pose.pose = poseMsg.pose;
		// odometryMsg.twist.twist = twistMsg.twist;
		// rosOdom.publish(odometryMsg);
		
		//wyślij ramkę (zakładamy ramkę map w 0,0,0)
		geometry_msgs::TransformStamped transMsg;
		transMsg.header.stamp = ros::Time::now();
		transMsg.header.frame_id = MAP_TF;
		transMsg.child_frame_id = "base_footprint";
		transMsg.transform.translation.x = pose.Pos().X();
		transMsg.transform.translation.y = pose.Pos().Y();
		transMsg.transform.translation.z = pose.Pos().Z();

		transMsg.transform.rotation.x = pose.Rot().X();
		transMsg.transform.rotation.y = pose.Rot().Y();
		transMsg.transform.rotation.z = pose.Rot().Z();
		transMsg.transform.rotation.w = pose.Rot().W();
		// to usunac dla filtru kalmana TODO
		// framePublisher.sendTransform(transMsg);

		tf::Quaternion rot = tf::createQuaternionFromRPY(0.0, 0.0, 0.0);

		// omnivelma_broadcaster.sendTransform(
		// tf::StampedTransform(
		// 	tf::Transform(rot, tf::Vector3(0.0, 0.0, 0.0)),
		// 	ros::Time::now(), "base_footprint", MODEL_NAME));


		//kinect
		
		ignition::math::Pose3d poseKinectR = model -> GetLink(linkPrefix + "ownKinect::" + "kinect_base") -> WorldPose();
		//poseKinectR.rot = model->WorldPose().Rot() * rkQ;
		poseKinectR.Set(poseKinectR.Pos(),model->WorldPose().Rot() * rkQ);
		model -> GetLink(linkPrefix + "ownKinect::" + "kinect_base") -> SetWorldPose(poseKinectR);

		geometry_msgs::TransformStamped transKRMsg;
		transKRMsg.header.stamp = ros::Time::now();
		transKRMsg.header.frame_id = MODEL_NAME;
		transKRMsg.child_frame_id = "camera_link_reoriented";
		//omnivelma's position
		transKRMsg.transform.translation.x = 0.0;
		transKRMsg.transform.translation.y = 0.0;
		transKRMsg.transform.translation.z = 1.5;

		transKRMsg.transform.rotation.x = rkQ.X();
		transKRMsg.transform.rotation.y = rkQ.Y();
		transKRMsg.transform.rotation.z = rkQ.Z();
		transKRMsg.transform.rotation.w = rkQ.W();

		framePublisher.sendTransform(transKRMsg);
		
		counter++;

		SetVelocities();
	}

	///Ustaw tarcia dla kół
	bool SetFriction(const omnivelma_msgs::SetFriction::Request& req, omnivelma_msgs::SetFriction::Response& res)
	{
		wheelRRCollision -> GetSurface() -> FrictionPyramid() -> SetMuPrimary(req.mu1);
		wheelRRCollision -> GetSurface() -> FrictionPyramid() -> SetMuSecondary(req.mu2);
		wheelRLCollision -> GetSurface() -> FrictionPyramid() -> SetMuPrimary(req.mu1);
		wheelRLCollision -> GetSurface() -> FrictionPyramid() -> SetMuSecondary(req.mu2);
		wheelFRCollision -> GetSurface() -> FrictionPyramid() -> SetMuPrimary(req.mu1);
		wheelFRCollision -> GetSurface() -> FrictionPyramid() -> SetMuSecondary(req.mu2);
		wheelFLCollision -> GetSurface() -> FrictionPyramid() -> SetMuPrimary(req.mu1);
		wheelFLCollision -> GetSurface() -> FrictionPyramid() -> SetMuSecondary(req.mu2);
		ROS_DEBUG("Ustawiono tarcia: %lf %lf", req.mu1, req.mu2);
		return true;
	}
	
	///Ustaw masy, środki mas i tensor inercji.
	bool SetInertia(const omnivelma_msgs::SetInertia::Request& req, omnivelma_msgs::SetInertia::Response& res)
	{
		physics::LinkPtr basePtr = model -> GetLink(linkPrefix + "base");
		physics::LinkPtr frontPtr = model -> GetLink(linkPrefix + "front");
		physics::LinkPtr wheelRRPtr = model -> GetLink(linkPrefix + "wheel_rr");
		physics::LinkPtr wheelRLPtr = model -> GetLink(linkPrefix + "wheel_rl");
		physics::LinkPtr wheelFRPtr = model -> GetLink(linkPrefix + "wheel_fr");
		physics::LinkPtr wheelFLPtr = model -> GetLink(linkPrefix + "wheel_fl");

		physics::InertialPtr baseIne = basePtr -> GetInertial();
		physics::InertialPtr frontIne = frontPtr -> GetInertial();
		physics::InertialPtr wheelRRIne = wheelRRPtr -> GetInertial();
		physics::InertialPtr wheelRLIne = wheelRLPtr -> GetInertial();
		physics::InertialPtr wheelFRIne = wheelFRPtr -> GetInertial();
		physics::InertialPtr wheelFLIne = wheelFLPtr -> GetInertial();

		baseIne -> SetMass(req.base.m);
		baseIne -> SetInertiaMatrix(req.base.ixx, req.base.iyy, req.base.izz, req.base.ixy, req.base.ixz, req.base.iyz);
		baseIne -> SetCoG(req.base.com.x, req.base.com.y, req.base.com.z);

		frontIne -> SetMass(req.front.m);
		frontIne -> SetInertiaMatrix(req.front.ixx, req.front.iyy, req.front.izz, req.front.ixy, req.front.ixz, req.front.iyz);
		frontIne -> SetCoG(req.front.com.x, req.front.com.y, req.front.com.z);

		wheelRRIne -> SetMass(req.wheel.m);
		wheelRRIne -> SetInertiaMatrix(req.wheel.ixx, req.wheel.iyy, req.wheel.izz, req.wheel.ixy, req.wheel.ixz, req.wheel.iyz);
		wheelRRIne -> SetCoG(req.wheel.com.x, req.wheel.com.y, req.wheel.com.z);
		wheelRLIne -> SetMass(req.wheel.m);
		wheelRLIne -> SetInertiaMatrix(req.wheel.ixx, req.wheel.iyy, req.wheel.izz, req.wheel.ixy, req.wheel.ixz, req.wheel.iyz);
		wheelRLIne -> SetCoG(req.wheel.com.x, req.wheel.com.y, req.wheel.com.z);
		wheelFRIne -> SetMass(req.wheel.m);
		wheelFRIne -> SetInertiaMatrix(req.wheel.ixx, req.wheel.iyy, req.wheel.izz, req.wheel.ixy, req.wheel.ixz, req.wheel.iyz);
		wheelFRIne -> SetCoG(req.wheel.com.x, req.wheel.com.y, req.wheel.com.z);
		wheelFLIne -> SetMass(req.wheel.m);
		wheelFLIne -> SetInertiaMatrix(req.wheel.ixx, req.wheel.iyy, req.wheel.izz, req.wheel.ixy, req.wheel.ixz, req.wheel.iyz);
		wheelFLIne -> SetCoG(req.wheel.com.x, req.wheel.com.y, req.wheel.com.z);

		//Liczenie masy z nowych macierzy może wywołać błędy, np. gdy masa jest zerowa
		try
		{
			frontPtr -> UpdateMass();
			basePtr -> UpdateMass();
			wheelRRPtr -> UpdateMass();
			wheelRLPtr -> UpdateMass();
			wheelFRPtr -> UpdateMass();
			wheelFLPtr -> UpdateMass();
		}
		catch(common::Exception err)
		{
			ROS_ERROR_STREAM("Nie udało się ustawić inercji: " << err.GetErrorStr());
			return false;
		}

		ROS_DEBUG("Ustawiono inercje");
		return true;
	}

	///Pobierz wiadomość od ROSa
	void OnRosMsg(const omnivelma_msgs::Vels::ConstPtr& msg)
	{
		velRR = msg -> rr;
		velRL = msg -> rl;
		velFR = msg -> fr;
		velFL = msg -> fl;
	
		SetVelocities();
	}

	void OnRosKinectMsg(const std_msgs::Float64ConstPtr & msg)
	{
		SetKinectRotation(msg->data);
	}

	///Wskaźnik na model
	physics::ModelPtr model;

	///Wskaźnik na zdarzenie aktualizacji
	event::ConnectionPtr updateConnection;

	///Przedrostek modelu
	std::string linkPrefix;

	///Motory kół
	physics::JointPtr motorRR;
	physics::JointPtr motorRL;
	physics::JointPtr motorFR;
	physics::JointPtr motorFL;
	
	//Kolizje kół
	physics::CollisionPtr wheelRRCollision;
	physics::CollisionPtr wheelRLCollision;
	physics::CollisionPtr wheelFRCollision;
	physics::CollisionPtr wheelFLCollision;

	double velRR;
	double velRL;
	double velFR;
	double velFL;

	///Node dla ROSa
	std::unique_ptr<ros::NodeHandle> rosNode;

	///Odbiornik prędkości kół
	ros::Subscriber rosSub;

	//PW
	//odbiornik zadanej predkosci obrotu kinecta
	ros::Subscriber rosKinectSub;

	///Nadajnik pozycji
	ros::Publisher rosPose;

	///Nadajnik enkodera
	ros::Publisher rosEnc;

	///Nadajnik prędkości
	ros::Publisher rosTwist;
	
	//Nadajnik Odometrii
	//Autor: Piotr Walas
	// ros::Publisher rosOdom;

	///Serwer ustawiania tarcia
	ros::ServiceServer rosFri;

	///Serwer ustawiania inercji
	ros::ServiceServer rosIne;

	///Licznik kroków
	unsigned int counter;
	
	//Nadajnik ramki
	tf2_ros::TransformBroadcaster framePublisher;

	tf::TransformBroadcaster omnivelma_broadcaster;

	ignition::math::Quaternion<double> rkP;
	ignition::math::Quaternion<double> rkQ;
	ignition::math::Quaternion<double> rkPN;
	ignition::math::Quaternion<double> rkQN;
	double ft = 0.1;

};

//zarejestruj wtyczkę
GZ_REGISTER_MODEL_PLUGIN(Omnivelma)
}
