# ign-omni

**NOTES**:
 - This repository is under development, you might find compilation errors,
malfunctions or some undocumented features.
 - This code will only run on Linux (for now).
 - You should [install Ignition Fortress](https://ignitionrobotics.org/docs/fortress) (from source).

**Requeriments**:
 - ROS 2 Galactic.
 - Ignition Gazebo Fortress

**Features**:
 - Ignition -> IssacSim
   - Move/rotate models
   - Create/remove models
   - Joints
   - Sensors (lidar, cameras)
 - IssacSim -> Ignition
   - Create/remove models

## How to compile it

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

## Run it

### Run Ignition

Run the `shapes.sdf` world in Ignition Gazebo. This should run in a separate terminal using your normal Ignition Gazebo installation.

```bash
ign gazebo -v 4 shapes.sdf
```

### Run IssacSim

Launch `IssacSim` and activate the `live sync`

![](live_sync.gif)

### Run the connector

And finally run:

**Note**: `ignition-omni` will be built under `src/ign-omni/_build`, this is because
it uses a custom build system by NVidia which is hard coded to put output in that directory.

In this case you need to source the special workspace that we have created
with the `ign-omni-meta` repository.

```bash
cd ~/ign-omni/src/ign-omni
bash run_ignition_omni.sh <name of the usd file to sync>
```

## Run a more complex simulation

For example you can run the turtlebot3. Compile the code from this PR https://github.com/ROBOTIS-GIT/turtlebot3_simulations/pull/180

Use ROS 2 Galactic.

```
mkdir -p ~/turtlebot3_ws/src
cd ~/turtlebot3_ws/src
git clone https://github.com/ahcorde/turtlebot3_simulations -b ahcorde/ignition_support
git clone https://github.com/ignitionrobotics/ign_ros2_control -b galactic
rosdep install --from-paths ./ -i -y --rosdistro galactic
```

Compile it

```bash
cd ~/turtlebot3_ws/
source /opt/ros/galactic/setup.sh
export IGNITION_VERSION=fortress
colcon build --merge-install --event-handlers console_direct+
```
### Run Ignition

This should run in a separate terminal using your normal Ignition Gazebo installation.

```bash
source ~/turtlebot3_ws/install/setup.bash
export TURTLEBOT3_MODEL=waffle ros2 launch turtlebot3_ignition ignition.launch.py
```

### Run IssacSim

Launch `IssacSim` and activate the `live sync`

![](live_sync.gif)

### Run the connector

Create this directory `omniverse://localhost/Users/ignition/` in the nucleus server  and run the connector

```bash
reset && bash run_ignition_omni.sh -p omniverse://localhost/Users/ignition/turtlebot3.usd -w empty -v
```

![](turtlebot3.gif)
