# Zephyr storage demos

## Setting up the environment
This project is being built as a [workspace](https://docs.zephyrproject.org/4.0.0/develop/application/index.html#zephyr-workspace-application) application.

1. Install the required dependencies as described in [Zephyr Getting Started#Install Dependencies](https://docs.zephyrproject.org/4.0.0/develop/getting_started/index.html#install-dependencies).

2. Clone the [zephyr-storage-demos](https://github.com/filipembedded/zephyr-storage-demos) repository into an empty directory (e.g. ```~/zsd-workspace/zephyr-storage-demos/```)

3. Create a new Python virtual environment in the workspace, install west and configure zephyr-related dependencies.

```
cd ~/zsd-workspace
python3 -m venv .venv
source .venv/bin/activate
pip install west
west init -l zephyr-storage-demos
west update
west zephyr-export
west packages pip --install
west sdk install
```
Note: On Windows, .venv activation is done different: 
```
.venv\Scipts\activate
```

## Demos
### lfs-boot-count-demo

This demo covers the basic LittleFS "boot count" example. It mounts filesystem, creates file boot_count, reads boot_count variable and increments that same variable by 1 on every another boot.

This demo supports use of two storage mediums: **SD card** and **NOR SPI flash**, and user can select which one he wants to use by setting specific Kconfig option. (There is no need to change source, just change Kconfig)

## Build
```
cd ~/zsd-workspace/zephyr-storage-demos
```
Configure the specific example using Kconfig (e.g. lfs-boot-count-demo):

```
west build -t menuconfig -b weact_stm32h5_core lfs-boot-count-demo
```
Build demo:
```
west build -b weact_stm32h5_core lfs-boot-count-demo
```