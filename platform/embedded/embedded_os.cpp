// embedded_os.cpp
#include "embedded_os.h"
#include "main/main.h"
#include "displayserver_embedded.h"

void EmbeddedOS::initialize() {
	OS_Unix::initialize_core();
}