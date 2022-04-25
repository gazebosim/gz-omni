# How to compile it

## Install Ignition

```bash
sudo apt update
sudo apt install python3-pip wget lsb-release gnupg curl
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
curl -s https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | sudo apt-key add -
sudo apt-get update
sudo apt-get install python3-vcstool python3-colcon-common-extensions
sudo apt-get install git libfreeimage-dev

sudo apt-get install ignition-edifice
```

For more information, see https://ignitionrobotics.org/docs/edifice/install_ubuntu_src.

# Compile ignition-omniverse

We need to compile some Ignition packages from source with a specific flag due the `omni-client` library.
To make this process simple we have created the [`ign-omni-meta` repository](https://github.com/ignitionrobotics/ign-omni-meta).

To compile this libraries you should run:

```bash
mkdir -p ~/ign-omni/src
cd ~/ign-omni/src
git clone https://github.com/ignitionrobotics/ign-omni-meta
vcs import . < ign-omni-meta/repos.yaml
cd protobuf
git -C . apply ../ign-omni-meta/protobuf-cmake.patch
cd ~/ign-omni
colcon build --merge-install --event-handlers console_direct+ --packages-select protobuf
cp src/ign-omni-meta/colcon.meta .
colcon build --merge-install --event-handlers console_direct+ --packages-up-to ignition-omniverse1
```

You can ignore the following message:

```bash
WARNING:colcon.colcon_cmake.task.cmake.build:Could not run installation step for package 'ignition-omniverse1' because it has no 'install' target
```

**Note: There will be 2 builds of ignition, the default build when ignition-edifice is compiled from source, and a special build with pre cxx11 abi compiled as part of ignition-omniverse.**
