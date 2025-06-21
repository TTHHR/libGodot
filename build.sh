export ANDROID_HOME="/home/harry/Android/Sdk/"
export ANDROID_NDK_ROOT="/home/harry/Android/Sdk/ndk/25.1.8937393/"
scons platform=linuxbsd dev_build=no target=template_release tools=no use_lto=yes module_websocket_enable=no module_upnp_enable=no module_webrtc_enable=no module_openxr_enabled=no -j12
