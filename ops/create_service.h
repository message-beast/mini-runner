#pragma once
#pragma optimize("03")
#pragma optimize("fast-math")
#pragma target("arch=native")
__attribute__((hot)) int save_services(service*** services);