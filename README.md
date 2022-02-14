# ign-omni

**NOTES**:
 - This repository is under development, you might find compilation errors,
malfunctions or some undocumented features.
 - This code will only run on Linux (for now).
 - You should [install Ignition Fortress](https://ignitionrobotics.org/docs/fortress) (from source or debs).

**Fatures**:
 - Ignition -> IssacSim
   - Move/rotate models
   - Create/remove models
   - Joints
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
colcon build --merge-install --event-handlers console_direct+ --packages-up-to ignition-omni1
```

## Run it

### Run Ignition

Run the `shapes.sdf` world in Ignition

```bash
ign gazebo v -4 shapes.sdf
```
### Run IssacSim

Launch `IssacSim` and activate the `live sync`

![](live_sync.gif)

### Run the connector

And finally run:

**Note**: `ignition-omni` will be built under `src/ign-omni/_build`, this is because
it uses a custom build system by NVidia which is hard coded to put output in that directory.

```bash
cd ~/ign-omni/src/ign-omni
bash run_ignition_omni.sh <name of the usd file to sync>
```
