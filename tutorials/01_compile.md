# How to compile it

## Compile ignition

```bash
sudo apt install python3-pip wget lsb-release gnupg curl
sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
curl -s https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | sudo apt-key add -
sudo apt-get update
sudo apt-get install python3-vcstool python3-colcon-common-extensions
sudo apt-get install git

# get sources
mkdir -p ~/ignition-fortress/src
cd ~/ignition-fortress/src
wget https://raw.githubusercontent.com/ignition-tooling/gazebodistro/master/collection-fortress.yaml
vcs import < collection-fortress.yaml

# install dependencies
sudo wget https://packages.osrfoundation.org/gazebo.gpg -O /usr/share/keyrings/pkgs-osrf-archive-keyring.gpg
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/pkgs-osrf-archive-keyring.gpg] http://packages.osrfoundation.org/gazebo/ubuntu-stable $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/gazebo-stable.list > /dev/null
sudo apt-get update
sudo apt -y install \
  $(sort -u $(find . -iname 'packages-'`lsb_release -cs`'.apt' -o -iname 'packages.apt' | grep -v '/\.git/') | sed '/ignition\|sdf/d' | tr '\n' ' ')

# build
cd ~/ignition-fortress
colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF --merge-install
```

For more information, see https://ignitionrobotics.org/docs/fortress/install_ubuntu_src.

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

**Note: There will be 2 builds of ignition, the default build when ignition-fortress is compiled from source, and a special build with pre cxx11 abi compiled as part of ignition-omniverse.**
